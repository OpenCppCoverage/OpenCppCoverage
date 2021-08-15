using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace TestMixedMode
{
    [TestClass]
    public class Tests
    {
        [TestMethod]
        public void TestCount()
        {
            var subject = new TestMixedModeLibrary.Mixed();
            Assert.AreEqual(12, subject.CountVowels("The quick brown fox jumped over the lazy dog."));
        }
    }
}