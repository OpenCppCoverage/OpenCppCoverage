& .\x64\Debug\CppCoverageTest.exe --gtest_filter=-CodeCoverageRunnerTest.OptimizedBuild
if ($LASTEXITCODE) {throw "CppCoverageTest failed"}

& .\x64\Debug\ExporterTest.exe
if ($LASTEXITCODE) {throw "ExporterTest failed"}

& .\x64\Debug\FileFilterTest.exe --gtest_filter=-RelocationsExtractorTest.Extract
if ($LASTEXITCODE) {throw "FileFilterTest failed"}

& .\x64\Debug\OpenCppCoverageTest.exe
if ($LASTEXITCODE) {throw "OpenCppCoverageTest failed"}

& .\x64\Debug\PluginTest.exe
if ($LASTEXITCODE) {throw "PluginTest failed"}

& .\x64\Debug\ToolsTest.exe
if ($LASTEXITCODE) {throw "ToolsTest failed"}