#!/bin/bash

# Dublin Protocol - Simple Virtual Environment Setup
# Creates venv in project root for easy VS Code integration

echo "🌌 Setting up Dublin Protocol virtual environment..."

# Check Python
if ! command -v python3 &> /dev/null; then
    echo "❌ Python3 not found"
    exit 1
fi

# Create venv in project root
echo "📦 Creating virtual environment..."
python3 -m venv venv

# Activate and install
echo "🔧 Installing dependencies..."
source venv/bin/activate
pip install --upgrade pip
pip install anthropic openai requests

# Create VS Code settings
echo "⚙️  Configuring VS Code..."
cat > .vscode/settings.json << 'EOF'
{
    "python.defaultInterpreterPath": "${workspaceFolder}/venv/bin/python",
    "python.terminal.activateEnvironment": true,
    "python.analysis.extraPaths": ["./tools"],
    "files.exclude": {
        "**/__pycache__": true,
        "**/*.pyc": true,
        "venv": true
    },
    "[python]": {
        "editor.defaultFormatter": "ms-python.black-formatter",
        "editor.formatOnSave": true,
        "editor.codeActionsOnSave": {
            "source.organizeImports": "explicit"
        }
    }
}
EOF

# Create VS Code extensions recommendation
cat > .vscode/extensions.json << 'EOF'
{
    "recommendations": [
        "ms-python.python",
        "ms-python.black-formatter",
        "ms-python.flake8",
        "ms-vscode.cpptools"
    ]
}
EOF

# Create simple activation script
cat > activate_venv.sh << 'EOF'
#!/bin/bash
source venv/bin/activate
echo "🌌 Dublin Protocol venv activated"
echo "Python: $(which python)"
EOF

chmod +x activate_venv.sh

# Create requirements
cat > requirements.txt << 'EOF'
anthropic>=0.25.0
openai>=1.0.0
requests>=2.31.0
EOF

echo ""
echo "✅ Setup complete!"
echo ""
echo "To activate:"
echo "  source venv/bin/activate"
echo "  # or: source activate_venv.sh"
echo ""
echo "VS Code will automatically detect the venv!"
echo ""
echo "Set your API key:"
echo "  export DEEPSEEK_API_KEY='your-key-here'"
echo ""
echo "Test:"
echo "  python tools/deepseek_interactive.py"