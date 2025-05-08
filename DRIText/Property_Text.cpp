#include "stdafx.h"
#include "Property_Text.h"
#include "DRITextObject.h"

const ACHAR* Property_Text::kMyAppName = _RXST("DRITextApp");
const ACHAR* Property_Text::kCategoryName = _RXST("DRITextCategory");

AcRxCategory* Property_Text::category = NULL;

Property_Text::Property_Text()
	: AcRxProperty(_T("Text"), AcRxValueType::Desc<AcString>::value())
{
	if (category == NULL)
	{
		AcRxCategory* parent = AcRxCategory::rootCategory();
		category = parent->findDescendant(kCategoryName);
		if (category == NULL)
			category = new AcRxCategory(kCategoryName, parent);
	}

	// OPM = Object Property Manager / Property Palette

	// Add the Placement attribute to set under which category in the OPM
	// the property will be shown 
	attributes().add(new AcRxUiPlacementAttribute(kCategoryName, 0));
	// Add this attribute so that AutoCAD will automatically create the COM
	// wrapper for this property, i.e. it will be visible in OPM
	attributes().add(new AcRxGenerateDynamicPropertiesAttribute());
}

Acad::ErrorStatus Property_Text::subGetValue(const AcRxObject* pO, AcRxValue& value) const
{
	// Get the value of the property from the object
	DRIText* driText = DRIText::cast(pO);
	if (driText == NULL)
		return Acad::eNotThatKindOfClass;
	
	AcString txt;
	Acad::ErrorStatus es = driText->textString(txt);
	if (es != Acad::eOk) return es;

	value = txt;
	return Acad::eOk;
}

Acad::ErrorStatus Property_Text::subSetValue(AcRxObject* pO, const AcRxValue& value) const
{
	DRIText* pEnt = DRIText::cast(pO);
	if (!pEnt)
		return Acad::eNotThatKindOfClass;

	// Extract the new string from the AcRxValue
	const AcString* newTxt = rxvalue_cast<AcString>(&value);
	if (!newTxt)
		return Acad::eInvalidInput;

	// Ensure we can write
	Acad::ErrorStatus es = pEnt->upgradeOpen();
	if (es != Acad::eOk) return es;

	// Update via the public setter
	return pEnt->setTextString(newTxt->kACharPtr());
}

AcRxObject* Property_Text::makePropertyText()
{
	// Create a new instance of the property
	return new Property_Text();
}