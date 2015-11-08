// OpenCppCoverage is an open source code coverage for C++.
// Copyright (C) 2014 OpenCppCoverage
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

using Microsoft.VisualStudio.TestTools.UnitTesting;
using Microsoft.VisualStudio.VCProjectEngine;
using Moq;
using OpenCppCoverage.VSPackage;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using VSPackage_UnitTests;

namespace VSPackage_IntegrationTests
{
    [TestClass()]
    public class SettingsBuilderTests
    {
        //---------------------------------------------------------------------
        static VCFile BuildVCFile(string path)
        {
            var file = new Mock<VCFile>();

            file.Setup(f => f.FullPath).Returns(path);
            return file.Object;
        }

        //---------------------------------------------------------------------
        [TestMethod]
        public void TestComputeCommonFolders()
        {
            var builder = new DteMockBuilder();

            const string file2 = @"C:\Dev\Folder2\Folder3\File2.txt";
            const string file4 = @"C:\Dev\Folder1\File4.txt";
            
            builder.VcFiles = new List<VCFile>
            {
                BuildVCFile(@"C:\Dev\Folder1\Folder2\Folder3\File1.txt"),
                BuildVCFile(file2),
                BuildVCFile(@"C:\Dev\Folder1\Folder2\File3.txt"),
                BuildVCFile(file4)               
            };
              
            var solution = builder.BuildSolutionMock();

            var settingsBuilder = new SettingsBuilder(solution.Object);
            var settings = settingsBuilder.ComputeSettings();

            var expectedFolders = new List<string> { 
                Path.GetDirectoryName(file4) + Path.DirectorySeparatorChar, 
                Path.GetDirectoryName(file2) + Path.DirectorySeparatorChar };
            CollectionAssert.AreEqual(expectedFolders, settings.SourcePaths.ToList());
        }
    }
}
