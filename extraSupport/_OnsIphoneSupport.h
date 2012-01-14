//
//  OnsIphoneSupport.h
//  ___PROJECTNAME___
//
//  Created by w s on 11-7-30.
//  Copyright 2011年 __MyCompanyName__. All rights reserved.
//

#ifndef ___PROJECTNAME____OnsIphoneSupport_h
#define ___PROJECTNAME____OnsIphoneSupport_h
class OnsIphoneSupport{
public:
    const char* OnsPath;
    void getOnsPath();
    int getHeight();
    int getWidth();
    int getInfo(const char* info,int v);//the info to query and v is the default value;
    void lg(const char* t);
};
#endif
