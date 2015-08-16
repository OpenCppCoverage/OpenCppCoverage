mkdir NewRelease
mkdir NewRelease\x86\Binaries
mkdir NewRelease\x86\Binaries\Template
xcopy /y /s /e Release\Template NewRelease\x86\Binaries\Template
xcopy /y Release\OpenCppCoverage.exe NewRelease\x86\Binaries
xcopy /y Release\Exporter.dll NewRelease\x86\Binaries
xcopy /y Release\CppCoverage.dll NewRelease\x86\Binaries
xcopy /y Release\Tools.dll NewRelease\x86\Binaries
xcopy /y Release\libctemplate.dll NewRelease\x86\Binaries
xcopy /y Release\template_test_util_test.dll NewRelease\x86\Binaries
xcopy /y Release\boost_filesystem-vc120-mt-1_55.dll NewRelease\x86\Binaries
xcopy /y Release\boost_program_options-vc120-mt-1_55.dll NewRelease\x86\Binaries
xcopy /y Release\boost_system-vc120-mt-1_55.dll NewRelease\x86\Binaries
xcopy /y Release\boost_log-vc120-mt-1_55.dll NewRelease\x86\Binaries
xcopy /y Release\boost_thread-vc120-mt-1_55.dll NewRelease\x86\Binaries
xcopy /y Release\boost_regex-vc120-mt-1_55.dll NewRelease\x86\Binaries
xcopy /y Release\boost_date_time-vc120-mt-1_55.dll NewRelease\x86\Binaries
xcopy /y Release\boost_locale-vc120-mt-1_55.dll NewRelease\x86\Binaries
xcopy /y Release\boost_chrono-vc120-mt-1_55.dll NewRelease\x86\Binaries

mkdir NewRelease\x86\Pdb
xcopy /y Release\OpenCppCoverage.pdb NewRelease\x86\Pdb
xcopy /y Release\Exporter.pdb NewRelease\x86\Pdb
xcopy /y Release\CppCoverage.pdb NewRelease\x86\Pdb
xcopy /y Release\Tools.pdb NewRelease\x86\Pdb

mkdir NewRelease\x64\Binaries
mkdir NewRelease\x64\Binaries\Template
xcopy /y /s /e Release\Template NewRelease\x64\Binaries\Template
xcopy /y x64\Release\OpenCppCoverage.exe NewRelease\x64\Binaries
xcopy /y x64\Release\Exporter.dll NewRelease\x64\Binaries
xcopy /y x64\Release\CppCoverage.dll NewRelease\x64\Binaries
xcopy /y x64\Release\Tools.dll NewRelease\x64\Binaries
xcopy /y x64\Release\libctemplate.dll NewRelease\x64\Binaries
xcopy /y x64\Release\template_test_util_test.dll NewRelease\x64\Binaries
xcopy /y x64\Release\boost_filesystem-vc120-mt-1_55.dll NewRelease\x64\Binaries
xcopy /y x64\Release\boost_program_options-vc120-mt-1_55.dll NewRelease\x64\Binaries
xcopy /y x64\Release\boost_system-vc120-mt-1_55.dll NewRelease\x64\Binaries
xcopy /y x64\Release\boost_log-vc120-mt-1_55.dll NewRelease\x64\Binaries
xcopy /y x64\Release\boost_thread-vc120-mt-1_55.dll NewRelease\x64\Binaries
xcopy /y x64\Release\boost_regex-vc120-mt-1_55.dll NewRelease\x64\Binaries
xcopy /y x64\Release\boost_date_time-vc120-mt-1_55.dll NewRelease\x64\Binaries
xcopy /y x64\Release\boost_locale-vc120-mt-1_55.dll NewRelease\x64\Binaries
xcopy /y x64\Release\boost_chrono-vc120-mt-1_55.dll NewRelease\x64\Binaries

mkdir NewRelease\x64\Pdb
xcopy /y x64\Release\OpenCppCoverage.pdb NewRelease\x64\Pdb
xcopy /y x64\Release\Exporter.pdb NewRelease\x64\Pdb
xcopy /y x64\Release\CppCoverage.pdb NewRelease\x64\Pdb
xcopy /y x64\Release\Tools.pdb NewRelease\x64\Pdb
pause