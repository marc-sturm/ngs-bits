#include "TestFramework.h"
#include "ServerHelper.h"
#include "ServerHelper.cpp"

TEST_CLASS(ServerHelper_Test)
{
Q_OBJECT
private slots:
	void test_generateToken()
	{
		QString token = ServerHelper::generateUniqueStr();
		I_EQUAL(token.length(), 36);
		I_EQUAL(token.count("-"), 4);
	}
};
