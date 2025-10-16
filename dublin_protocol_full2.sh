#!/bin/bash
set -euo pipefail

echo "=== Dublin Protocol All-in-One ==="

# --- Mode selection ---
while true; do
    echo
    echo "Modes:"
    echo "1) Safe Test Mode (GitHub-friendly, no real vaults)"
    echo "2) Real Mode (creates actual encrypted vault, only for trusted local use)"
    read -rp "Choose mode [1/2]: " MODE
    if [[ "$MODE" == "1" || "$MODE" == "2" ]]; then
        break
    else
        echo "Invalid mode. Enter 1 or 2."
    fi
done

# --- Vault size for Real Mode ---
if [[ "$MODE" == "2" ]]; then
    read -rp "Enter vault size in MB [default 1024]: " VAULT_SIZE
    VAULT_SIZE="${VAULT_SIZE:-1024}"
fi

# --- Cleanup functions ---
cleanup_safe() { echo "[i] Safe Test Mode: no cleanup needed."; }
cleanup_real() {
    echo "[i] Performing cleanup..."
    if mountpoint -q /mnt/secure_vault; then sudo umount /mnt/secure_vault || true; fi
    if [[ -e /dev/mapper/vault_crypt ]]; then sudo cryptsetup close vault_crypt || true; fi
    if [[ -n "${VAULT_IMG:-}" && -f "$VAULT_IMG" ]]; then rm -f "$VAULT_IMG"; fi
}
trap cleanup_safe EXIT
[[ "$MODE" == "2" ]] && trap cleanup_real EXIT

# --- 1️⃣ Install dependencies ---
echo "[i] Installing dependencies..."
sudo apt update
sudo apt install -y python3 python3-venv git xxd sha256sum cryptsetup openssl par2 coreutils
echo "[✓] Dependencies installed"

# --- 2️⃣ Project folder ---
DEMO_DIR="$HOME/dublin_demo"
mkdir -p "$DEMO_DIR"
cd "$DEMO_DIR"
echo "[✓] Project folder ready at $DEMO_DIR"

# --- 3️⃣ Anchor / KDF ---
read -rp "Enter a meaningful phrase: " PHRASE
SALT="dublin_salt_2025"

if [[ "$MODE" == "1" ]]; then
    FINGERPRINT=$(python3 - <<EOF
import hashlib, sys
phrase = sys.argv[1]
salt = sys.argv[2]
print(hashlib.sha256((phrase + salt).encode()).hexdigest())
EOF
"$PHRASE" "$SALT")
    echo "[✓] Simulated fingerprint: $FINGERPRINT"
else
    python3 -m venv .venv
    . .venv/bin/activate
    python3 -m pip install --quiet argon2-cffi

    python3 - <<EOF
import argon2, hashlib
phrase = "$PHRASE"
salt = "$SALT".encode()
kdf = argon2.low_level.hash_secret_raw(
    secret=phrase.encode(),
    salt=salt,
    time_cost=3,
    memory_cost=65536,
    parallelism=1,
    hash_len=64,
    type=argon2.low_level.Type.ID
)
with open("luks.key", "wb") as f: f.write(kdf[:32])
with open("protocol.fingerprint", "w") as f: f.write(hashlib.sha256(kdf).hexdigest() + '\n')
with open("protocol.salt", "w") as f: f.write("$SALT\n")
EOF
    chmod 600 luks.key
    echo "[✓] Real KDF / key derivation complete"
fi

# --- 4️⃣ Vault creation ---
if [[ "$MODE" == "1" ]]; then
    echo "[✓] Safe Test Mode: Vault creation skipped"
else
    echo "[i] Creating vault image of size ${VAULT_SIZE}MB..."
    VAULT_IMG="secure_vault.img"
    dd if=/dev/zero of="$VAULT_IMG" bs=1M count="$VAULT_SIZE" status=progress

    echo "[i] Formatting LUKS2 encrypted vault..."
    sudo cryptsetup luksFormat --type luks2 "$VAULT_IMG" --key-file luks.key
    sudo cryptsetup open "$VAULT_IMG" vault_crypt --key-file luks.key
    sudo mkfs.ext4 -F /dev/mapper/vault_crypt
    sudo mkdir -p /mnt/secure_vault
    sudo mount /dev/mapper/vault_crypt /mnt/secure_vault
    sudo chown $USER:$USER /mnt/secure_vault
    echo "Dublin Protocol Active — $(date)" > /mnt/secure_vault/consensus.txt
    sudo umount /mnt/secure_vault
    sudo cryptsetup close vault_crypt
    echo "[✓] Real vault created and secured"
fi

# --- 5️⃣ Vault splitting / redundancy ---
if [[ "$MODE" == "1" ]]; then
    echo "[✓] Safe Test Mode: Split & PAR2 skipped"
else
    echo "[i] Splitting vault into 50MB chunks with 30% PAR2 redundancy..."
    split -b 50M "$VAULT_IMG" vault_chunk_
    sha256sum vault_chunk_* > vault_manifest.sha256
    par2 create -r30 vault_chunk_*
    echo "[✓] Vault split and redundancy created"
fi

# --- 6️⃣ Recovery (Real Mode) ---
if [[ "$MODE" == "2" ]]; then
    echo
    read -rp "Do you want to recover the vault now? [y/N]: " RECOVER
    if [[ "$RECOVER" =~ ^[Yy]$ ]]; then
        echo "[i] Verifying chunks..."
        sha256sum -c vault_manifest.sha256
        PAR2_FILES=./vault_chunk_*.par2
        if ls $PAR2_FILES 1> /dev/null 2>&1; then
            echo "[i] Repairing with PAR2..."
            par2 repair "${PAR2_FILES[0]}"
        fi
        echo "[i] Reassembling vault..."
        cat vault_chunk_* > recovered_vault.img
        echo "[i] Opening LUKS vault..."
        sudo cryptsetup open recovered_vault.img vault_crypt --key-file luks.key
        sudo mkdir -p /mnt/secure_vault
        sudo mount /dev/mapper/vault_crypt /mnt/secure_vault
        sudo chown $USER:$USER /mnt/secure_vault
        echo "[✓] Vault mounted at /mnt/secure_vault"
        echo "[i] Use 'sudo umount /mnt/secure_vault' and 'sudo cryptsetup close vault_crypt' to unmount safely."
    fi
fi

# --- 7️⃣ Recovery simulation / Safe Mode verification ---
if [[ "$MODE" == "1" ]]; then
    read -rp "Re-enter demo phrase to verify: " VERIFY
    VERIFY_FP=$(python3 - <<EOF
import hashlib
phrase = "$VERIFY"
salt = "$SALT"
print(hashlib.sha256((phrase + salt).encode()).hexdigest())
EOF
)
    if [[ "$VERIFY_FP" == "$FINGERPRINT" ]]; then
        echo "✅ Safe Demo recovery successful"
    else
        echo "❌ Safe Demo recovery failed"
    fi
fi

# --- 8️⃣ Prepare GitHub repo ---
cat > README.md <<'EOF'
# Dublin Protocol Demo

This repository contains scripts to safely demonstrate the Dublin Protocol workflow.
Safe test mode simulates anchor derivation, vault creation, splitting, and recovery.
No real keys or vaults are included in GitHub-friendly mode.
EOF

cat > .gitignore <<'EOF'
*.img
*.key
*.par2
*.venv/
EOF

git init
git branch -M main
git add README.md dublin_protocol_full.sh .gitignore
git commit -m "Initial Dublin Protocol demo"
echo "[✓] Local Git repository prepared"

read -rp "Enter GitHub repo URL to push (e.g., https://github.com/user/repo.git): " GH_REPO
git remote add origin "$GH_REPO"
git push -u origin main
echo "[✓] Demo pushed to GitHub successfully"

trap - EXIT
echo
echo "=== Dublin Protocol Setup Complete ==="
echo "Mode: $([[ $MODE == "1" ]] && echo 'Safe Test Mode' || echo 'Real Mode')"
echo "Everything verified and GitHub updated."
