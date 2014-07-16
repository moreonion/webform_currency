Purpose of this module
======================

webform_country_list is a simple addon module for the webform module that
defines a new webform component. It is compatible with webform version 3.

It's a webform component that lets the user select a country out of a list that
you can configure.

To configure, you choose a set of countries (out of the complete list of all
countries) that can be selected. You can also change the order in which they
appear in the select box.

If the PHP PECL package geoip is available and it detects the country
of the visitor, and this country is in the list of available countries,
webform_country_list preselect the geoip country as default.

Related modules
===============

[countries] https://www.drupal.org/project/countries

Configuration
=============

When adding a new component to your webform choose 'Country List' as type.

When editing the component check the 'available' checkbox for all countries that
you want to include in the select box.
By draging and dropping the "cross" sign you can also change the order in which
the countries appear in the select box.

Installing
==========

Nothing special, if you're using drush that would be

drush dl webform_country_list

drush en webform_country_list -y
