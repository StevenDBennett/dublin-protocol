#!/bin/bash

# Dublin Protocol - Environment Setup Script
# For Ubuntu 25.04

echo "🌌 Setting up Dublin Protocol research environment..."

# Check if Python3 is available
if ! command -v python3 &> /dev/null; then
    echo "❌ Python3 not found. Please install Python 3.8+"
    exit 1
fi

# Create virtual environment
echo "📦 Creating virtual environment..."
python3 -m venv dublin_env

# Activate virtual environment
echo "🔧 Activating virtual environment..."
source dublin_env/bin/activate

# Upgrade pip
echo "⬆️  Upgrading pip..."
pip install --upgrade pip

# Install dependencies
echo "📚 Installing Python dependencies..."
pip install anthropic openai requests

# Install system dependencies for C++ compilation
echo "🔧 Installing system dependencies..."
sudo apt update
sudo apt install -y g++ build-essential cmake

# Create activation script
echo "📝 Creating activation script..."
cat > activate_dublin.sh << 'EOF'
#!/bin/bash
# Dublin Protocol Activation Script

source dublin_env/bin/activate
export DUBLIN_PROTOCOL_ROOT="$(pwd)"

echo "🌌 Dublin Protocol environment activated!"
echo "Project root: $DUBLIN_PROTOCOL_ROOT"
echo "Python: $(which python)"
echo ""
echo "Available tools:"
echo "  python tools/deepseek_interactive.py    - Interactive chat with context"
echo "  python tools/session_manager.py --list  - View chat sessions"
echo "  ./dublin_protocol [simulation]          - Run simulations"
echo ""
EOF

chmod +x activate_dublin.sh

# Create requirements file
echo "📋 Creating requirements file..."
cat > requirements.txt << 'EOF'
# Dublin Protocol Research Dependencies
anthropic>=0.25.0
openai>=1.0.0
requests>=2.31.0
EOF

# Create development setup script
echo "⚙️  Creating development setup..."
cat > setup_development.sh << 'EOF'
#!/bin/bash
# Development setup for Dublin Protocol

source dublin_env/bin/activate

# Install development dependencies
pip install black flake8 mypy pytest

# Create git hooks directory
mkdir -p .git/hooks

# Create pre-commit hook for code quality
cat > .git/hooks/pre-commit << 'GITHOOK'
#!/bin/bash
# Dublin Protocol pre-commit hook

echo "🔍 Running code quality checks..."

# Check Python files
python_files=$(git diff --cached --name-only --diff-filter=ACM | grep '\.py$')

if [ -n "$python_files" ]; then
    echo "Checking Python files..."
    black --check $python_files
    flake8 $python_files
fi

# Check C++ files
cpp_files=$(git diff --cached --name-only --diff-filter=ACM | grep '\.cpp$')

if [ -n "$cpp_files" ]; then
    echo "Checking C++ files..."
    for file in $cpp_files; do
        echo "Compiling $file..."
        g++ -std=c++23 -c "$file" -o /dev/null 2>/dev/null
        if [ $? -ne 0 ]; then
            echo "❌ Compilation failed for $file"
            exit 1
        fi
    done
fi

echo "✅ All checks passed!"
GITHOOK

chmod +x .git/hooks/pre-commit

echo "✅ Development environment setup complete!"
EOF

chmod +x setup_development.sh

echo ""
echo "✅ Dublin Protocol environment setup complete!"
echo ""
echo "To activate the environment:"
echo "  source activate_dublin.sh"
echo ""
echo "To set up development tools:"
echo "  ./setup_development.sh"
echo ""
echo "Remember to set your DeepSeek API key:"
echo "  export DEEPSEEK_API_KEY='your-api-key-here'"
echo ""

# Test the setup
echo "🧪 Testing setup..."
source dublin_env/bin/activate
python -c "import anthropic; print('✅ Anthropic library installed')"
python -c "import openai; print('✅ OpenAI library installed')"

echo ""
echo "🌌 Dublin Protocol research environment is ready!"
echo "You can now use the interactive chat tools with persistent context."