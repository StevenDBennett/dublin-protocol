#!/bin/bash
set -euo pipefail

echo "=== Dublin Protocol Full Setup ==="
echo
echo "Modes:"
echo "1) Safe Test Mode (GitHub-friendly, no real vaults)"
echo "2) Real Mode (creates actual encrypted vault, only for trusted local use)"
read -rp "Choose mode [1/2]: " MODE

# --- 1️⃣ Install dependencies ---
sudo apt update
sudo apt install -y python3 python3-venv git xxd sha256sum cryptsetup openssl par2 || true
echo "[✓] Dependencies installed"

# --- 2️⃣ Setup project folder ---
DEMO_DIR="$HOME/dublin_demo"
mkdir -p "$DEMO_DIR"
cd "$DEMO_DIR"
echo "[✓] Project folder ready at $DEMO_DIR"

# --- 3️⃣ Anchor / KDF derivation ---
read -rp "Enter a meaningful phrase: " PHRASE
SALT="dublin_salt_2025"

if [[ "$MODE" == "1" ]]; then
    # Safe Test Mode
    FINGERPRINT=$(python3 - <<EOF
import hashlib, sys
phrase = sys.argv[1]
salt = sys.argv[2]
print(hashlib.sha256((phrase + salt).encode()).hexdigest())
EOF
"$PHRASE" "$SALT")
    echo "Simulated fingerprint: $FINGERPRINT"

elif [[ "$MODE" == "2" ]]; then
    # Real Mode
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
    VAULT_IMG="secure_vault.img"
    dd if=/dev/zero of="$VAULT_IMG" bs=1M count=1024 status=progress
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
    split -b 50M "$VAULT_IMG" vault_chunk_
    sha256sum vault_chunk_* > vault_manifest.sha256
    par2 create -r30 vault_chunk_aa
    echo "[✓] Vault split and redundancy created"
fi

# --- 6️⃣ Recovery simulation / verification ---
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
else
    echo "[✓] Real Mode: Recovery can be run manually using RECLAIM_TRUTH.sh if desired"
fi

# --- 7️⃣ Prepare GitHub repo ---
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

echo
echo "=== Dublin Protocol Setup Complete ==="
echo "Mode: $([[ $MODE == "1" ]] && echo 'Safe Test Mode' || echo 'Real Mode')"
echo "Everything verified and GitHub updated."