bool BaseMultipleFieldsDateAndTimeInputType::shouldSpinButtonRespondToWheelEvents()
{
    if (!shouldSpinButtonRespondToMouseEvents())
        return false;
    if (DateTimeEditElement* edit = dateTimeEditElement())
        return edit->hasFocusedField();
    return false;
}
