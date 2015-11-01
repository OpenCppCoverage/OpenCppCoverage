import zipfile
import shutil
import os
from optparse import OptionParser


#------------------------------------------------------------------------------
def get_options(script_directory):
    parser = OptionParser()
    parser.add_option(
        "--archive",
        dest="archive",
        default=os.path.join(script_directory, 'Dependencies-vs2015.zip'),
        help="The dependencies archive.")
    (options, args) = parser.parse_args()
    return options


#------------------------------------------------------------------------------
def extract_files_to(zip_file, base_folder_to_extract, destination_folder):
    if not os.path.exists(destination_folder):
        os.makedirs(destination_folder)

    for name in zip_file.namelist():
        filename = os.path.basename(name)
        if name.startswith(base_folder_to_extract) and filename:
            with zip_file.open(name) as f:
                destination_path = os.path.join(destination_folder, filename)
                with open(destination_path, 'wb') as dest:
                    shutil.copyfileobj(f, dest)


#------------------------------------------------------------------------------
def main():
    script_directory = os.path.dirname(os.path.abspath(__file__))
    options = get_options(script_directory)
    archive = options.archive
    repository_root = os.path.abspath(os.path.join(script_directory, '..', '..'))
    vspackage_folder = os.path.join(repository_root, 'VSPackage')

    with zipfile.ZipFile(archive) as f:
        f.extractall(repository_root)
        extract_files_to(f, 'Release', os.path.join(vspackage_folder, 'OpenCppCoverage-x86'))
        extract_files_to(f, 'x64/Release', os.path.join(vspackage_folder, 'OpenCppCoverage-x64'))


#------------------------------------------------------------------------------
if __name__ == '__main__':
    main()