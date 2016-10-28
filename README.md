PURPOSE OF THIS MODULE
======================

webform_currency is a simple addon module for the webform module that
defines a new webform component.

It's a webform component that lets the user select a currency out of a list that
you can configure.

To configure, you choose a set of currencies (out of the complete list of all
currencies) that can be selected. You can also change the order in which they
appear in the select box.

CONFIGURATION
=============

When adding a new component to your webform choose 'Currency' as type.

When editing the component check the 'available' checkbox for all currencies that
you want to include in the select box.
By draging and dropping the "cross" sign you can also change the order in which
the currencies appear in the select box.

If no currency is selected when editing the component, all currencies will be
available in the webform.


INSTALLING
==========

Nothing special, if you're using drush that would be

drush dl webform_currency

drush en webform_currency -y

You can find more information about installing contributed modules here:
https://www.drupal.org/documentation/install/modules-themes/modules-7


DEPENDENCIES
============

Hard dependencies:
   * webform (version 4)

Soft dependencies:
   * form_builder

