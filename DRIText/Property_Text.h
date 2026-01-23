#pragma once

#include "rxmember.h"
#include "rxvaluetype.h"
#include "rxattrib.h"
#include "rxprop.h"
#include "rxcategory.h"

class Property_Text : public AcRxProperty
{
public:
	static AcRxObject* makePropertyText();

	static const ACHAR* kMyAppName;
	static const ACHAR* kCategoryName;
	static AcRxCategory* category;

	Property_Text();
	~Property_Text() {}

	virtual Acad::ErrorStatus subGetValue(const AcRxObject* pO, AcRxValue& value) const;
	virtual Acad::ErrorStatus subSetValue(AcRxObject* pO, const AcRxValue& value) const;
};