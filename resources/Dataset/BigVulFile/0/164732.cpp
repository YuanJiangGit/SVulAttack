  void TestClearTwoSections(const char* html, bool unowned) {
    LoadHTML(html);
    WebLocalFrame* web_frame = GetMainFrame();
    ASSERT_NE(nullptr, web_frame);

    FormCache form_cache(web_frame);
    std::vector<FormData> forms = form_cache.ExtractNewForms();
    ASSERT_EQ(1U, forms.size());

    WebInputElement firstname_shipping =
        GetInputElementById("firstname-shipping");
    firstname_shipping.SetAutofillValue("John");
    firstname_shipping.SetAutofillState(WebAutofillState::kAutofilled);
    firstname_shipping.SetAutofillSection("shipping");

    WebInputElement lastname_shipping =
        GetInputElementById("lastname-shipping");
    lastname_shipping.SetAutofillValue("Smith");
    lastname_shipping.SetAutofillState(WebAutofillState::kAutofilled);
    lastname_shipping.SetAutofillSection("shipping");

    WebInputElement city_shipping = GetInputElementById("city-shipping");
    city_shipping.SetAutofillValue("Montreal");
    city_shipping.SetAutofillState(WebAutofillState::kAutofilled);
    city_shipping.SetAutofillSection("shipping");

    WebInputElement firstname_billing =
        GetInputElementById("firstname-billing");
    firstname_billing.SetAutofillValue("John");
    firstname_billing.SetAutofillState(WebAutofillState::kAutofilled);
    firstname_billing.SetAutofillSection("billing");

    WebInputElement lastname_billing = GetInputElementById("lastname-billing");
    lastname_billing.SetAutofillValue("Smith");
    lastname_billing.SetAutofillState(WebAutofillState::kAutofilled);
    lastname_billing.SetAutofillSection("billing");

    WebInputElement city_billing = GetInputElementById("city-billing");
    city_billing.SetAutofillValue("Paris");
    city_billing.SetAutofillState(WebAutofillState::kAutofilled);
    city_billing.SetAutofillSection("billing");

    EXPECT_TRUE(form_cache.ClearSectionWithElement(firstname_shipping));

    EXPECT_FALSE(firstname_shipping.IsAutofilled());
    EXPECT_FALSE(lastname_shipping.IsAutofilled());
    EXPECT_FALSE(city_shipping.IsAutofilled());
    EXPECT_TRUE(firstname_billing.IsAutofilled());
    EXPECT_TRUE(lastname_billing.IsAutofilled());
    EXPECT_TRUE(city_billing.IsAutofilled());

    FormData form;
    FormFieldData field;
    EXPECT_TRUE(FindFormAndFieldForFormControlElement(firstname_shipping, &form,
                                                      &field));
    EXPECT_EQ(GetCanonicalOriginForDocument(web_frame->GetDocument()),
              form.origin);
    EXPECT_FALSE(form.origin.is_empty());
    if (!unowned) {
      EXPECT_EQ(ASCIIToUTF16("TestForm"), form.name);
      EXPECT_EQ(GURL("http://abc.com"), form.action);
    }

    const std::vector<FormFieldData>& fields = form.fields;
    ASSERT_EQ(6U, fields.size());

    FormFieldData expected;
    expected.form_control_type = "text";
    expected.max_length = WebInputElement::DefaultMaxLength();

    expected.is_autofilled = false;
    expected.id_attribute = ASCIIToUTF16("firstname-shipping");
    expected.name = expected.id_attribute;
    EXPECT_FORM_FIELD_DATA_EQUALS(expected, fields[0]);

    expected.id_attribute = ASCIIToUTF16("lastname-shipping");
    expected.name = expected.id_attribute;
    EXPECT_FORM_FIELD_DATA_EQUALS(expected, fields[1]);

    expected.id_attribute = ASCIIToUTF16("city-shipping");
    expected.name = expected.id_attribute;
    EXPECT_FORM_FIELD_DATA_EQUALS(expected, fields[2]);

    expected.is_autofilled = true;
    expected.id_attribute = ASCIIToUTF16("firstname-billing");
    expected.name = expected.id_attribute;
    expected.value = ASCIIToUTF16("John");
    EXPECT_FORM_FIELD_DATA_EQUALS(expected, fields[3]);

    expected.id_attribute = ASCIIToUTF16("lastname-billing");
    expected.name = expected.id_attribute;
    expected.value = ASCIIToUTF16("Smith");
    EXPECT_FORM_FIELD_DATA_EQUALS(expected, fields[4]);

    expected.id_attribute = ASCIIToUTF16("city-billing");
    expected.name = expected.id_attribute;
    expected.value = ASCIIToUTF16("Paris");
    EXPECT_FORM_FIELD_DATA_EQUALS(expected, fields[5]);

    EXPECT_EQ(0, firstname_shipping.SelectionStart());
    EXPECT_EQ(0, firstname_shipping.SelectionEnd());
  }
