/*********************************************
 * utilsTest.h
 * Author: kamuszhou@tencent.com kamuszhou@qq.com
 * website: v.qq.com  www.dogeye.net
 * Created on: Aug 5, 2014
 * Praise Be to the Lord. BUG-FREE CODE !
 ********************************************/

#ifndef UTILSTEST_H_
#define UTILSTEST_H_

#include "gtest/gtest.h"

class UtilsTest : public ::testing::Test
{
public:
	UtilsTest();
	virtual ~UtilsTest();

protected:
	virtual void SetUp();
	virtual void TearDown();
};

#endif /* UTILSTEST_H_ */
