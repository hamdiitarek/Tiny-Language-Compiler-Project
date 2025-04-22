import sys
import os
import subprocess
from PySide6.QtWidgets import (QApplication, QMainWindow, QWidget, QVBoxLayout, 
                           QHBoxLayout, QTextEdit, QPushButton, QLabel, 
                           QFileDialog, QTabWidget, QSplitter, QMessageBox)
from PySide6.QtGui import QPixmap, QFont
from PySide6.QtCore import Qt, QProcess

class TinyCompilerGUI(QMainWindow):
    def __init__(self):
        super().__init__()
        self.initUI()
        
    def initUI(self):
        self.setWindowTitle('TINY Language Compiler')
        self.setGeometry(100, 100, 1200, 800)
        
        # Main widget and layout
        main_widget = QWidget()
        main_layout = QVBoxLayout()
        main_widget.setLayout(main_layout)
        self.setCentralWidget(main_widget)
        
        # Top panel: Code input and controls
        top_panel = QWidget()
        top_layout = QVBoxLayout()
        top_panel.setLayout(top_layout)
        
        # Code editor
        editor_label = QLabel("TINY Code Editor:")
        self.code_editor = QTextEdit()
        self.code_editor.setFont(QFont("Courier New", 10))
        self.code_editor.setPlaceholderText("Enter your TINY code here or open a file...")
        
        # Sample code
        sample_code = "read x;\nif x < 0 then\n  fact := 1;\nrepeat\n  fact := fact * x;\n  x := x - 1;\nuntil x = 0;\nwrite fact;\nend"
        self.code_editor.setText(sample_code)
        
        # Buttons panel
        buttons_layout = QHBoxLayout()
        
        self.open_btn = QPushButton("Open File")
        self.save_btn = QPushButton("Save Code")
        self.compile_btn = QPushButton("Compile")
        
        self.open_btn.clicked.connect(self.open_file)
        self.save_btn.clicked.connect(self.save_file)
        self.compile_btn.clicked.connect(self.compile_code)
        
        buttons_layout.addWidget(self.open_btn)
        buttons_layout.addWidget(self.save_btn)
        buttons_layout.addWidget(self.compile_btn)
        
        # Add to top layout
        top_layout.addWidget(editor_label)
        top_layout.addWidget(self.code_editor)
        top_layout.addLayout(buttons_layout)
        
        # Bottom panel: Results with tabs
        self.tab_widget = QTabWidget()
        
        # Tokens tab
        self.tokens_text = QTextEdit()
        self.tokens_text.setReadOnly(True)
        self.tokens_text.setFont(QFont("Courier New", 10))
        self.tab_widget.addTab(self.tokens_text, "Tokens")
        
        # Console output tab
        self.console_output = QTextEdit()
        self.console_output.setReadOnly(True)
        self.console_output.setFont(QFont("Courier New", 10))
        self.tab_widget.addTab(self.console_output, "Console Output")
        
        # Syntax Tree tab
        self.tree_widget = QLabel("Compile code to see syntax tree")
        self.tree_widget.setAlignment(Qt.AlignmentFlag.AlignCenter)
        scroll_area = QWidget()
        scroll_layout = QVBoxLayout()
        scroll_layout.addWidget(self.tree_widget)
        scroll_area.setLayout(scroll_layout)
        self.tab_widget.addTab(scroll_area, "Syntax Tree")
        
        # Create a splitter to allow resizing
        splitter = QSplitter(Qt.Orientation.Vertical)
        splitter.addWidget(top_panel)
        splitter.addWidget(self.tab_widget)
        splitter.setSizes([400, 400])
        
        main_layout.addWidget(splitter)
        
        # Process for running external commands
        self.process = QProcess()
        self.process.readyReadStandardOutput.connect(self.handle_stdout)
        self.process.readyReadStandardError.connect(self.handle_stderr)
        self.process.finished.connect(self.process_finished)
        
        self.show()
    
    def open_file(self):
        file_path, _ = QFileDialog.getOpenFileName(self, "Open TINY File", "", "TINY Files (*.tiny);;All Files (*)")
        if file_path:
            try:
                with open(file_path, 'r') as file:
                    self.code_editor.setText(file.read())
                self.console_output.append(f"File opened: {file_path}")
            except Exception as e:
                QMessageBox.critical(self, "Error", f"Could not open file: {str(e)}")
    
    def save_file(self):
        file_path, _ = QFileDialog.getSaveFileName(self, "Save TINY File", "", "TINY Files (*.tiny);;All Files (*)")
        if file_path:
            try:
                with open(file_path, 'w') as file:
                    file.write(self.code_editor.toPlainText())
                self.console_output.append(f"File saved: {file_path}")
            except Exception as e:
                QMessageBox.critical(self, "Error", f"Could not save file: {str(e)}")
    
    def compile_code(self):
        # Clear previous outputs
        self.tokens_text.clear()
        self.console_output.clear()
        self.tree_widget.setText("Processing...")
        try:
            import tempfile
            import os
            self.temp_dir = tempfile.TemporaryDirectory()
            temp_dir_path = self.temp_dir.name
            input_tiny_path = os.path.join(temp_dir_path, "input.tiny")
            with open(input_tiny_path, 'w') as file:
                file.write(self.code_editor.toPlainText())
            self.console_output.append(f"Temporary directory: {temp_dir_path}")
            self.input_tiny_path = input_tiny_path
            self.run_compilation_steps(temp_dir_path)
        except Exception as e:
            self.console_output.append(f"Error: {str(e)}")
            QMessageBox.critical(self, "Error", f"Compilation error: {str(e)}")
    
    def run_compilation_steps(self, temp_dir_path):
        """Run compilation steps manually in temp dir"""
        import shutil
        import os
        import subprocess
        try:
            # Check and copy necessary files to temp dir
            required_files = ["scanner.c", "parser.c", "token_strings.c", "tokens.h", "Makefile"]
            missing_files = []
            # Determine source directory for required files
            if hasattr(sys, '_MEIPASS'):
                source_dir = sys._MEIPASS
            else:
                source_dir = os.getcwd()
            for fname in required_files:
                src_path = os.path.join(source_dir, fname)
                if os.path.exists(src_path):
                    shutil.copy(src_path, temp_dir_path)
                else:
                    missing_files.append(fname)
            if missing_files:
                self.console_output.append(f"Error: Missing required files: {', '.join(missing_files)}. Please ensure all necessary source files are present in the project directory.")
                self.tree_widget.setText("Compilation aborted due to missing files.")
                return
            # Write input.tiny already done
            cwd = os.getcwd()
            os.chdir(temp_dir_path)
            # Create Makefile if not exists
            if not os.path.exists("Makefile"):
                with open("Makefile", 'w') as file:
                    file.write("""
.PHONY: all clean scanner parser

all: scanner parser

scanner: scanner.c token_strings.c tokens.h
	gcc -Wall -o scanner scanner.c token_strings.c

parser: parser.c token_strings.c tokens.h
	gcc -Wall -o parser parser.c token_strings.c

clean:
	rm -f scanner parser tokens.txt tree.dot tree.png
""")
                self.console_output.append("Created Makefile in temp dir")
            # Compile
            self.console_output.append("Compiling scanner and parser...")
            compile_output = subprocess.run(["make", "clean", "all"], capture_output=True, text=True)
            self.console_output.append("[make stdout]\n" + compile_output.stdout)
            if compile_output.stderr:
                self.console_output.append("[make stderr]\n" + compile_output.stderr)
            if compile_output.returncode != 0:
                self.console_output.append(f"[make] exited with code {compile_output.returncode}")
                raise subprocess.CalledProcessError(compile_output.returncode, compile_output.args, output=compile_output.stdout, stderr=compile_output.stderr)
            # Run scanner
            self.console_output.append("Running scanner...")
            scanner_output = subprocess.run(["./scanner",], check=True, capture_output=True, text=True)
            self.console_output.append(scanner_output.stdout)
            if scanner_output.stderr:
                self.console_output.append(scanner_output.stderr)
            # Display tokens
            tokens_path = os.path.join(temp_dir_path, "tokens.txt")
            if os.path.exists(tokens_path):
                with open(tokens_path, 'r') as file:
                    token_content = file.read()
                    self.tokens_text.setText(token_content)
                    self.console_output.append("Tokens generated successfully.")
            else:
                self.console_output.append("Warning: tokens.txt was not generated")
            # Run parser
            self.console_output.append("Running parser...")
            parser_output = subprocess.run(["./parser",], check=True, capture_output=True, text=True)
            self.console_output.append(parser_output.stdout)
            if parser_output.stderr:
                self.console_output.append(parser_output.stderr)
            # Generate visualization
            tree_dot_path = os.path.join(temp_dir_path, "tree.dot")
            tree_png_path = os.path.join(temp_dir_path, "tree.png")
            if os.path.exists(tree_dot_path):
                self.console_output.append("Generating visualization...")
                viz_output = subprocess.run(["dot", "-Tpng", "tree.dot", "-o", "tree.png"], check=True, capture_output=True, text=True)
                if viz_output.stderr:
                    self.console_output.append(viz_output.stderr)
                # Display the syntax tree
                self.update_syntax_tree(tree_png_path)
                self.console_output.append("Compilation completed successfully!")
            else:
                self.console_output.append("Warning: tree.dot was not generated")
            os.chdir(cwd)
        except subprocess.CalledProcessError as e:
            self.console_output.append(f"Error in compilation process: {e}")
            if hasattr(e, 'stderr') and e.stderr:
                self.console_output.append(f"Error output: {e.stderr}")
            if hasattr(e, 'stdout') and e.stdout:
                self.console_output.append(f"Output: {e.stdout}")
            QMessageBox.critical(self, "Compilation Error", f"Error in compilation process: {e}")
            os.chdir(cwd)
        except Exception as e:
            self.console_output.append(f"Unexpected error: {str(e)}")
            QMessageBox.critical(self, "Error", f"Unexpected error: {str(e)}")
            os.chdir(cwd)
    
    def handle_stdout(self):
        data = self.process.readAllStandardOutput().data().decode()
        self.console_output.append(data)
        
        # Check if tokens.txt has been generated
        if os.path.exists("tokens.txt"):
            try:
                with open("tokens.txt", 'r') as file:
                    token_content = file.read()
                    self.tokens_text.setText(token_content)
            except Exception as e:
                self.console_output.append(f"Error reading tokens: {str(e)}")
    
    def handle_stderr(self):
        data = self.process.readAllStandardError().data().decode()
        self.console_output.append(f"ERROR: {data}")
    
    def process_finished(self, exit_code, exit_status):
        if exit_code == 0:
            self.console_output.append("Compilation completed successfully!")
            self.update_syntax_tree()
        else:
            self.console_output.append(f"Compilation failed with exit code {exit_code}")
            self.tree_widget.setText("Compilation failed. No syntax tree available.")
    
    def update_syntax_tree(self, tree_png_path=None):
        if tree_png_path is None:
            tree_png_path = os.path.join(getattr(self, 'temp_dir', tempfile.gettempdir()), "tree.png")
        if os.path.exists(tree_png_path):
            pixmap = QPixmap(tree_png_path)
            self.tree_widget.setPixmap(pixmap)
            self.tree_widget.setScaledContents(False)
            self.tab_widget.setCurrentIndex(2)
        else:
            self.tree_widget.setText("No syntax tree image generated")

if __name__ == '__main__':
    app = QApplication(sys.argv)
    window = TinyCompilerGUI()
    sys.exit(app.exec())