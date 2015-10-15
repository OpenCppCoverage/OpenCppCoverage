import os
import re
import shutil
import fileinput
import subprocess
from optparse import OptionParser


#------------------------------------------------------------------------------
def get_opencppcoverage_version(repository_root):
    default_props = os.path.join(repository_root, 'PropertySheets', 'Default.props')
    with open(default_props, 'r') as content_file:
        content = content_file.read()
        m = re.search('OPENCPPCOVERAGE_VERSION="([0-9\.]*)"', content)
        return m.group(1)


#------------------------------------------------------------------------------
def get_options():
    parser = OptionParser()
    parser.add_option("--out_dir", dest="out_dir", help="Destination diretory")
    (options, args) = parser.parse_args()
    return options


#------------------------------------------------------------------------------
def get_installer_files(iss_path):
    files = []
    with open(iss_path, 'r') as content_file:
        content = content_file.read()
        for match in re.finditer(r'binaries\\([^\\;]*)"; DestDir', content):
            files.append(match.group(1))
    return files


#------------------------------------------------------------------------------
def copy_file(source, destination):
    print('Copy {} -> {}'.format(source, destination))
    shutil.copy(source, destination)


#------------------------------------------------------------------------------
def copy_folder(source, destination):
    print('Copy {} -> {}'.format(source, destination))
    shutil.copytree(source, destination)


#------------------------------------------------------------------------------
def update_iss_file(version_path, version, architecture, iss_template_path):
    iss_path = '{}-{}.iss'.format(iss_template_path, architecture)
    with open(iss_template_path, 'r') as template_content:
        with open(iss_path, 'w') as content:
            for line in template_content:
                line = line.replace('{{VERSION}}', version)
                line = line.replace('{{ARCHITECTURE}}', architecture)
                x64_mode = 'ArchitecturesInstallIn64BitMode=x64' if architecture == 'x64' else ''
                line = line.replace('{{ARCHITECTURES_INSTALL_IN_64_BITS_MODE}}', x64_mode)
                content.write(line)
    return iss_path


#------------------------------------------------------------------------------
def create_installer(iss_path):
    print('Create Installer')
    subprocess.check_call(['iscc', '/q', iss_path])


#------------------------------------------------------------------------------
def create_release(version, architecture, version_path, binary_directory, iss_template_path):
    arch_path = os.path.join(version_path, architecture)
    os.mkdir(arch_path)

    binaries_path = os.path.join(arch_path, 'binaries')
    os.mkdir(binaries_path)
    copy_folder(
        os.path.join(binary_directory, 'Template'),
        os.path.join(binaries_path, 'Template'))

    for file in get_installer_files(iss_template_path):
        copy_file(
            os.path.join(binary_directory, file),
            os.path.join(binaries_path, file))

    pdbs_path = os.path.join(arch_path, 'pdbs')
    os.mkdir(pdbs_path )
    for file in ['OpenCppCoverage.pdb', 'Exporter.pdb', 'CppCoverage.pdb', 'Tools.pdb']:
        copy_file(
            os.path.join(binary_directory, file),
            os.path.join(pdbs_path , file))
    iss_path = update_iss_file(version_path, version, architecture, iss_template_path)
    create_installer(iss_path)


#------------------------------------------------------------------------------
if __name__ == '__main__':
    options = get_options()
    script_directory = os.path.dirname(os.path.abspath(__file__))
    repository_root = os.path.abspath(os.path.join(script_directory, '..', '..'))
    iss_template_path = os.path.join(script_directory, 'Installer-Template.iss')
    version = get_opencppcoverage_version(repository_root)
    version_path = os.path.join(options.out_dir, version)

    os.mkdir(version_path)
    create_release(version, 'x86', version_path, os.path.join(repository_root, 'Release'), iss_template_path)
    create_release(version, 'x64', version_path, os.path.join(repository_root, 'x64', 'Release'), iss_template_path)
