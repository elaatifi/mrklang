#include "Common.h"

#ifdef MRK_MAIN

#include <vector>
#include <iostream>
#include <string>

int main()
{
	mrks cout << "ملوك الشر اهلا\nالشوكة رقم 9\nادخل رقم الشر:";
	int دخول_الشر;
	mrks cin >> دخول_الشر;

	for (int الاندكس = 0; الاندكس < دخول_الشر; ++الاندكس)
		mrks cout << "نحن الاشرار, الشوكة رقم " << الاندكس + 1 << '\n';
	
	system("pause");
	return 0;
}

#endif