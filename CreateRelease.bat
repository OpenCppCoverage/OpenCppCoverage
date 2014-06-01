mkdir NewRelease
mkdir NewRelease\Binaries
mkdir NewRelease\Binaries\Template
xcopy /y /s /e Release\Template NewRelease\Binaries\Template
xcopy /y Release\OpenCppCoverage.exe NewRelease\Binaries
xcopy /y Release\Exporter.dll NewRelease\Binaries
xcopy /y Release\CppCoverage.dll NewRelease\Binaries
xcopy /y Release\Tools.dll NewRelease\Binaries
xcopy /y Release\libctemplate.dll NewRelease\Binaries
xcopy /y Release\template_test_util_test.exe NewRelease\Binaries
xcopy /y Release\boost_filesystem-vc120-mt-1_55.dll NewRelease\Binaries
xcopy /y Release\boost_program_options-vc120-mt-1_55.dll NewRelease\Binaries
xcopy /y Release\boost_system-vc120-mt-1_55.dll NewRelease\Binaries
xcopy /y Release\boost_log-vc120-mt-1_55.dll NewRelease\Binaries
xcopy /y Release\boost_thread-vc120-mt-1_55.dll NewRelease\Binaries
xcopy /y Release\boost_regex-vc120-mt-1_55.dll NewRelease\Binaries
xcopy /y Release\boost_date_time-vc120-mt-1_55.dll NewRelease\Binaries
xcopy /y Release\boost_locale-vc120-mt-1_55.dll NewRelease\Binaries
xcopy /y Release\boost_chrono-vc120-mt-1_55.dll NewRelease\Binaries

mkdir NewRelease\Pdb
xcopy /y Release\OpenCppCoverage.pdb NewRelease\Pdb
xcopy /y Release\Exporter.pdb NewRelease\Pdb
xcopy /y Release\CppCoverage.pdb NewRelease\Pdb
xcopy /y Release\Tools.pdb NewRelease\Pdb