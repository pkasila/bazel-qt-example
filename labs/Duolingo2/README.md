# LinguistPro - Premium Language Learning Application

## Features
- **Modern Duolingo-style UI** with custom QSS styling
- **Translation exercises** with fuzzy string matching
- **Grammar exercises** with multiple choice questions
- **Advanced Damerau-Levenshtein algorithm** for typo tolerance
- **Context-sensitive help system** (press H for hints)
- **Audio feedback** with correct/wrong sound effects
- **Difficulty settings** (Beginner/Student/Linguist modes)
- **Real-time timer and lives system**

## Build Requirements
- Qt6 development packages
- Bazel build system
- Ubuntu/Debian Linux

## Installation

### 1. Install Qt6 dependencies
```bash
sudo apt update
sudo apt install qt6-base-dev qt6-tools-dev-tools qt6-multimedia-dev
sudo apt install libqt6multimedia6-plugins gstreamer1.0-plugins-base gstreamer1.0-plugins-good gstreamer1.0-pulseaudio gstreamer1.0-alsa
```

### 2. Add sound files (optional)
Place your audio files in the `assets/` folder:
- `assets/correct.wav` - sound for correct answers
- `assets/wrong.wav` - sound for wrong answers

**Note:** Files must be in uncompressed WAV format. If you don't have these files, the app will still work without sound.

## Build and Run

```bash
# Build the application
bazel build //:LanguageApp

# Run the application
bazel run //:LanguageApp
```

## How to Use

1. **Choose your exercise type:**
   - 🌍 Translation Training - Translate English phrases to Russian
   - 📚 Grammar Practice - Choose correct grammar options

2. **Difficulty Settings:**
   - Новичок (Beginner): 5 lives, 60 seconds
   - Студент (Student): 3 lives, 30 seconds  
   - Лингвист (Linguist): 1 life, 15 seconds

3. **Get Help:**
   - Press `H` key during exercises to see context-sensitive hints

4. **Scoring:**
   - +150 points for each correct answer
   - Lose one life for each wrong answer
   - Game ends when lives run out or time expires

## Technical Features

### Advanced String Matching
The app uses the **Damerau-Levenshtein algorithm** which:
- Ignores case and punctuation
- Allows for typos and character transpositions
- Dynamically adjusts tolerance based on word length
- Provides intelligent fuzzy matching

### Modern UI Design
- Custom QSS styling with Duolingo color scheme
- Smooth button press animations
- Responsive layout with proper spacing
- Emoji icons for visual appeal

### Audio System
- Qt6 Multimedia integration
- Safe fallback if audio files are missing
- Debug output for troubleshooting audio issues

## Project Structure
```
Duolingo2/
├── WORKSPACE                 # Bazel workspace configuration
├── BUILD.bazel              # Bazel build configuration with MOC
├── assets/                  # Audio files (correct.wav, wrong.wav)
├── src/
│   ├── main.cpp            # Application entry point
│   ├── MainWindow.h/cpp    # Main application window
│   ├── DifficultyDialog.h/cpp # Settings dialog
│   ├── Tasks.h             # Exercise database
│   └── Levenshtein.h       # Advanced string matching
└── README.md               # This file
```

## Troubleshooting

### Audio Not Working
1. Check that files are in proper WAV format: `file assets/correct.wav`
2. Verify Qt6 multimedia plugins are installed
3. Check debug output in terminal for file path issues

### Build Issues
1. Ensure Qt6 development packages are installed
2. Check that MOC path is correct in BUILD.bazel
3. Verify all source files are present

This application demonstrates advanced Qt programming with Bazel build system, featuring a complete language learning experience with professional UI design and intelligent text processing.
