import os
import sys

SYSTEM_CALLS = "System Calls"
ALL_PROJECTS = "All Projects"
REPO = "https://github.com/iiTONELOC/DoubleSea.git"

PROJECTS = [
    "Scheduler",
    "Messenger",
    SYSTEM_CALLS,
    ALL_PROJECTS,
]

PROJECT_LOCATIONS = {
    "Scheduler": "../THREADS-Scheduler",
    "Messenger": "../THREADS-Messenger",
    SYSTEM_CALLS: "../THREADS-SystemCalls",
    ALL_PROJECTS: [
        "../THREADS-Scheduler",
        "../THREADS-Messenger",
        "../THREADS-SystemCalls",
    ],
}


# might have to change this depending on the version of Visual Studio installed
# this is the path for Visual Studio 2022 Build Tools - which is what I have installed
VS_DEV_CMD_PATH = "C:/Program Files (x86)/Microsoft Visual Studio/2022/BuildTools/Common7/Tools/VsDevCmd.bat"


def printLogo():
    # Generated with https://patorjk.com/software/taag
    asciiLogo = r"""
  _____              _     _         _____              _      _ _     
 |  __ \            | |   | |       / ____|            | |    (_) |    
 | |  | | ___  _   _| |__ | | ___  | (___   ___  __ _  | |     _| |__  
 | |  | |/ _ \| | | | '_ \| |/ _ \  \___ \ / _ \/ _` | | |    | | '_ \ 
 | |__| | (_) | |_| | |_) | |  __/  ____) |  __/ (_| | | |____| | |_) |
 |_____/ \___/ \__,_|_.__/|_|\___| |_____/ \___|\__,_| |______|_|_.__/ 
                                                                       """
    print(asciiLogo)


def CatchKeyboardInterrupt(func):
    """Decorator to catch KeyboardInterrupt and exit gracefully

    Args:
        func (Callable): The function to decorate
    """

    def wrapper(*args, **kwargs):
        try:
            return func(*args, **kwargs)
        except KeyboardInterrupt:
            print("\nOperation cancelled by user.")
            sys.exit(1)

    return wrapper


def promptForDir(dir: str | None = None) -> str:
    """Prompt for and validate the installation directory

    Args:
        dir (str | None, optional): A provided dir to validate. Defaults to None.

    Returns:
        str: The validated directory path
    """
    # Prompt user for installation directory
    installDir = (
        dir
        if dir
        else input("Enter the directory where you would like to install DoubleSeaLib: ")
    )

    # check for tilde and expand
    if installDir.startswith("~"):
        installDir = os.path.expanduser(installDir)

    # validate input
    if not os.path.isdir(installDir):
        print("Invalid directory")
        sys.exit(1)

    return os.path.abspath(installDir)


@CatchKeyboardInterrupt
def determineProjectToAttachLibTo() -> str:
    """Prompt the user to select the project to attach the library to"""

    global PROJECTS
    while True:
        print("\nFor which project would you like to attach the library to?")
        for i, project in enumerate(PROJECTS):
            print(f"  {i+1}. {project}")

        choice = input("  \nEnter the number for the corresponding project: ")
        try:
            choice = int(choice)
            if choice < 1 or choice > len(PROJECTS):
                print("Invalid choice")
                continue
            return PROJECTS[choice - 1]
        except ValueError:
            print("Invalid choice")
            continue


def cloneRepo(toInstallDir: str):
    """Clones the DoubleSea repository to the installation directory or updates it if it already exists"""
    # check for DoubleSea directory
    if os.path.isdir(f"{toInstallDir}/DoubleSea"):
        print("DoubleSea directory already exists, updating...")
        os.system(f"cd {toInstallDir}/DoubleSea && git fetch && git pull origin main")
    else:
        print(f"Cloning {REPO} to {toInstallDir}...")
        os.system(f"cd {toInstallDir} && git clone {REPO}")


def copyFilesToProject(toInstallDir: str, projectDir: str):
    """Copies the necessary files to the project directory

    Args:
        toInstallDir (str): the installation directory
        projectDir (str): the project directory to copy the files to
    """
    headerSrc = f'"{toInstallDir}\\DoubleSea\\DoubleSeaLib.h"'
    headerDst = f'"{projectDir}\\Include"'
    libSrc = f'"{toInstallDir}\\DoubleSea\\x64\\Release\\DoubleSeaLib.lib"'
    dllSrc = f'"{toInstallDir}\\DoubleSea\\x64\\Release\\DoubleSeaLib.dll"'
    libDst = f'"{projectDir}\\bin"'
    dllDst = f'"{projectDir}\\bin"'

    os.system(command=f"copy {headerSrc} {headerDst}")
    os.system(f"copy {libSrc} {libDst}")
    os.system(f"copy {dllSrc} {dllDst}")


def buildSeaLib(toInstallDir: str, forProject: str):
    """Builds the DoubleSea library and copies the necessary files to the project directory"""
    print("\nBuilding DoubleSeaLib...")

    bat_script = os.path.join(toInstallDir, "build_doublesea.bat")

    with open(bat_script, "w") as f:
        f.write(f'"{VS_DEV_CMD_PATH}" && ')
        f.write(f'cd "{toInstallDir}\\DoubleSea" && ')
        f.write('msbuild DoubleSeaLib.sln /p:Configuration=Release /p:Platform="x64"\n')

    # Run the batch file
    os.system(f'cmd /c "{bat_script}"')

    # Clean up the batch file if desired
    os.remove(bat_script)

    # Copy built files
    projectDirs = PROJECT_LOCATIONS[forProject]
    print(f"\n Copying files to {forProject}...")
    if forProject == ALL_PROJECTS:
        for projectDir in projectDirs:
            copyFilesToProject(toInstallDir, projectDir)
    else:
        copyFilesToProject(toInstallDir, projectDirs)
    print("Files copied successfully!")


@CatchKeyboardInterrupt
def installDoubleSea(dir: str | None = None):
    printLogo()

    # Get the installation directory
    installDir = promptForDir(dir)

    # Determine the project to attach the library to
    project = determineProjectToAttachLibTo()

    print(f"\nDownloading DoubleSeaLib to {installDir} for {project}, please wait...")

    # Clone the repository
    cloneRepo(installDir)

    # Build the library and copy the necessary files to the project directory
    buildSeaLib(installDir, project)
    print("\nInstallation Complete!")


def main():
    installDoubleSea()


if __name__ == "__main__":
    main()
