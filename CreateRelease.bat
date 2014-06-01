mkdir NewRelease
mkdir NewRelease\Template
xcopy /y /s /e Release\Template NewRelease\Template
xcopy /y Release\OpenCppCoverage.exe NewRelease
xcopy /y Release\Exporter.dll NewRelease
xcopy /y Release\CppCoverage.dll NewRelease
xcopy /y Release\Tools.dll NewRelease
xcopy /y Release\libctemplate.dll NewRelease
xcopy /y Release\template_test_util_test.exe NewRelease
xcopy /y Release\boost_filesystem-vc120-mt-1_55.dll NewRelease
xcopy /y Release\boost_program_options-vc120-mt-1_55.dll NewRelease
xcopy /y Release\boost_system-vc120-mt-1_55.dll NewRelease
xcopy /y Release\boost_log-vc120-mt-1_55.dll NewRelease
xcopy /y Release\boost_thread-vc120-mt-1_55.dll NewRelease
xcopy /y Release\boost_regex-vc120-mt-1_55.dll NewRelease
xcopy /y Release\boost_date_time-vc120-mt-1_55.dll NewRelease
xcopy /y Release\boost_locale-vc120-mt-1_55.dll NewRelease
xcopy /y Release\boost_chrono-vc120-mt-1_55.dll NewRelease

mkdir NewReleasePdb
xcopy /y Release\OpenCppCoverage.pdb NewReleasePdb
xcopy /y Release\Exporter.pdb NewReleasePdb
xcopy /y Release\CppCoverage.pdb NewReleasePdb
xcopy /y Release\Tools.pdb NewReleasePdb