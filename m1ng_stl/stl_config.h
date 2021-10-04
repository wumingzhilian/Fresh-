#pragma once
#ifndef __STL_CONFIG_H__


#define __STL_STL_CONFIG_H__

//这里是SGI版本stl为了兼容性
#define __STL_BEGIN_NAMESPACE namespace ding_stl{
#define __STL_END_NAMESPACE }
/*
__STL_BEGIN_NAMESPACE宏是在某个配置文件中定义的，就sgi来说，此宏为了兼容一些早期代码，允许stl模板库不是用std命名空间包裹，__STL_BEGIN_NAMESPACE根据用户配置，被定义为“空”或者“namespace std {”之类的实际代码。
*/

#endif // !__STL_CONFIG_H__

