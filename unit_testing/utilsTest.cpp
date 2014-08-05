/*********************************************
 * utilsTest.cpp
 * Author: kamuszhou@tencent.com kamuszhou@qq.com
 * website: v.qq.com  www.dogeye.net
 * Created on: Aug 5, 2014
 * Praise Be to the Lord. BUG-FREE CODE !
 ********************************************/

#include "utilsTest.h"
#include "utils.h"

UtilsTest::UtilsTest()
{
}

UtilsTest::~UtilsTest()
{
}

void UtilsTest::SetUp()
{
}

void UtilsTest::TearDown()
{
}

TEST_F(UtilsTest, create_port_test)
{
	std::string id("12");
	unsigned short port = create_port(id);

	ASSERT_EQ(port, 0x3231);
}
