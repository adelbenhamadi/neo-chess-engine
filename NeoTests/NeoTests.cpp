#include "CppUnitTest.h"
#include "../libNeoEngine/board.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace NeoTests
{
	TEST_CLASS(NeoTests)
	{
	public:
		
		TEST_METHOD(Board_setup)
		{
			Logger::WriteMessage("TESTING -> board.setup()");
			std::string fen = "8/p2pQ2p/2p1p2k/4Bqp1/2P2P2/P6P/6PK/3r4 w - - 1 0";
			Board b(fen);
			const Square s = b.getKingSquare(true);
			Assert::IsTrue(s == Square::SQ_F2);
		
		}
	};
}

////////////////////////////////////////////////////////////
/* USAGE EXAMPLE
// The following is an example of VSCppUnit usage.
// It includes examples of attribute metadata, fixtures,
// unit tests with assertions, and custom logging.


#include <CppUnitTest.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

BEGIN_TEST_MODULE_ATTRIBUTE()
	TEST_MODULE_ATTRIBUTE(L"Date", L"2010/6/12")
END_TEST_MODULE_ATTRIBUTE()


TEST_MODULE_INITIALIZE(ModuleInitialize)
{
	Logger::WriteMessage("In Module Initialize");
}

TEST_MODULE_CLEANUP(ModuleCleanup)
{
	Logger::WriteMessage("In Module Cleanup");
}



TEST_CLASS(Class1)
{
public:
	Class1()
	{
		Logger::WriteMessage("In Class1");
	}
	~Class1()
	{
		Logger::WriteMessage("In ~Class1");
	}



	TEST_CLASS_INITIALIZE(ClassInitialize)
	{
		Logger::WriteMessage("In Class Initialize");
	}
	TEST_CLASS_CLEANUP(ClassCleanup)
	{
		Logger::WriteMessage("In Class Cleanup");
	}


	BEGIN_TEST_METHOD_ATTRIBUTE(Method1)
		TEST_OWNER(L"OwnerName")
		TEST_PRIORITY(1)
	END_TEST_METHOD_ATTRIBUTE()

	TEST_METHOD(Method1)
	{
		Logger::WriteMessage("In Method1");
		Assert::AreEqual(0, 0);
	}

	TEST_METHOD(Method2)
	{
		Assert::Fail(L"Fail");
	}
};
*/