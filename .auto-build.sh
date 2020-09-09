#/bin/bash

# Automatic build script
#
# To run locally, execute:
# sudo apt install doxygen graphviz texlive-latex-base texlive-latex-recommended texlive-pictures texlive-latex-extra

# Exit immediately if a command exits with a non-zero status.
set -e

echo "Starting auto-build script..."


function autobuild()
{
    # Set environment variables
    BOARDS_AVR="--board uno --board megaatmega2560 --board leonardo"
    BOARDS_ARM="--board due"
    BOARDS_ESP="--board d1_mini --board nodemcuv2 --board lolin_d32"

    # echo "Installing library dependencies"
    platformio lib --global install https://github.com/Erriez/ErriezSerialTerminal

    echo "Building examples..."
    platformio ci --lib="examples/ErriezDS1307Alarm" --lib="." ${BOARDS_AVR} ${BOARDS_ESP} examples/ErriezDS1307Alarm/ErriezDS1307Alarm.ino
    platformio ci --lib="." ${BOARDS_AVR} ${BOARDS_ARM} ${BOARDS_ESP} examples/ErriezDS1307Bare/ErriezDS1307Bare.ino
    platformio ci --lib="." ${BOARDS_AVR} ${BOARDS_ARM} ${BOARDS_ESP} examples/ErriezDS1307DumpRegisters/ErriezDS1307DumpRegisters.ino
    platformio ci --lib="." ${BOARDS_AVR} ${BOARDS_ARM} ${BOARDS_ESP} examples/ErriezDS1307SetBuildDateTime/ErriezDS1307SetBuildDateTime.ino
    platformio ci --lib="." ${BOARDS_AVR} ${BOARDS_ARM} ${BOARDS_ESP} examples/ErriezDS1307SetGetDateTime/ErriezDS1307SetGetDateTime.ino
    platformio ci --lib="." ${BOARDS_AVR} ${BOARDS_ARM} ${BOARDS_ESP} examples/ErriezDS1307SetGetTime/ErriezDS1307SetGetTime.ino
    platformio ci --lib="." ${BOARDS_AVR} ${BOARDS_ARM} ${BOARDS_ESP} examples/ErriezDS1307SQWInterrupt/ErriezDS1307SQWInterrupt.ino
    platformio ci --lib="." ${BOARDS_AVR} ${BOARDS_ARM} ${BOARDS_ESP} examples/ErriezDS1307Terminal/ErriezDS1307Terminal.ino
    platformio ci --lib="." ${BOARDS_AVR} ${BOARDS_ARM} ${BOARDS_ESP} examples/ErriezDS1307Test/ErriezDS1307Test.ino
    platformio ci --lib="." ${BOARDS_AVR} ${BOARDS_ARM} ${BOARDS_ESP} examples/ErriezDS1307WriteRead/ErriezDS1307WriteRead.ino
}

function generate_doxygen()
{
    echo "Generate Doxygen HTML..."

    DOXYGEN_PDF="ErriezDS1307.pdf"

    # Cleanup
    rm -rf html latex

    # Generate Doxygen HTML and Latex
    doxygen Doxyfile

    # Allow filenames starting with an underscore    
    echo "" > html/.nojekyll

    # Generate PDF when script is not running on Travis-CI
    if [[ -z ${TRAVIS_BUILD_DIR} ]]; then
        # Generate Doxygen PDF
        make -C latex

        # Copy PDF to root directory
        cp latex/refman.pdf ./${DOXYGEN_PDF}
    fi
}

autobuild
generate_doxygen
