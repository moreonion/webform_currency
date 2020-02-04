<?php

/**
 * @file
 * Implementation of the webform component using the webform API.
 */

// Necessary for currency_get_list()
include_once DRUPAL_ROOT . '/includes/locale.inc';

// Some functionality is leveraged from the generic select component.
require_once drupal_get_path('module', 'webform') . '/components/select.inc';

/**
 * Implements _webform_defaults_[component]().
 *
 * @return array
 *   An webform component array initialized with default values.
 */
function _webform_defaults_currency() {
  $select = _webform_defaults_select();
  $element = array(
    'name'      => 'Currency',
    'form_key'  => NULL,
    'mandatory' => 0,
    'required'  => 0,
    'value'     => '',
    'weight' => 0,
    'extra'     => array(
      'options_enabled' => array(),
      'private' => FALSE,
      'optrand' => FALSE,
      'empty_option' => NULL,
    ) + $select['extra'],
  );

  return $element;
}

/**
 * Helper function to choose items from an associative array.
 *
 * This is almost the same as `array_intersect_key($all_options, $selection)`
 * except that it uses the order of $selection instead of $all_options.
 *
 * @param array $all_options
 *   Array containing available values and their labels.
 * @param array $selection
 *   Sorted array of values that which’s options should be returned.
 *
 * @param array
 *   All options from $all_options which’s keys are present in the $selection
 *   in the order of their occurence in $selection.
 */
function _webform_currency_choose_options(array $all_options, array $selection) {
  $options = [];
  foreach ($selection as $key) {
    if (isset($all_options[$key])) {
      $options[$key] = $selection[$key];
    }
  }
  return $options;
}

/**
 * Implements _webform_edit_[component]().
 *
 * Every currency components stores the list of currencies selected by the
 * user (available currencies) in that order in the DB. Only the ISO codes are
 * saved in the DB.
 *
 * @param array $component
 *   A Webform component array.
 *
 * @return array
 *   An array of form items to be displayed on the edit component page.
 */
function _webform_edit_currency(array $component) {
  $component['extra']['items'] = '';
  $component['extra']['aslist'] = TRUE;
  $component['extra']['multiple'] = FALSE;
  $component['extra']['options_source'] = NULL;
  $component['extra']['custom_keys'] = FALSE;
  $form = _webform_edit_select($component);
  $form['display']['aslist']['#access'] = FALSE;
  unset($form['items']);

  $form['extra']['options_enabled'] = array(
    '#tree' => TRUE,
    '#type' => 'container',
    '#theme' => 'currency_form',
    '#attached' => array(
      'css' => array(drupal_get_path('module', 'webform_currency') . '/webform_currency.css'),
    ),
    '#element_validate' => ['_webform_currency_edit_validate'],
  );
  // Get the list of currencies previously stored for this component.
  // The order is the same as in the DB.
  $all_options = webform_currency_options();
  $enabled_options = $component['extra']['options_enabled'];
  // Only currency ISO codes are stored in the DB, get the currency names.
  $options = _webform_currency_choose_options($all_options, $enabled_options);
  // Prepare the complete currency list: first the previously selected currencies,
  // then the rest.
  $options += $all_options;
  $total = count($options);
  $delta = 0;
  foreach ($options as $key => $label) {
    $form_entry = &$form['extra']['options_enabled'][$key];
    $form_entry = array(
      '#type' => 'container',
    );
    $form_entry['available'] = array(
      '#type'          => 'checkbox',
      '#return_value'  => $key,
      '#default_value' => isset($enabled_options[$key]) ? $key : NULL,
      '#attributes'    => array('class' => array('option-available')),
    );
    $form_entry['currency_code'] = array(
      '#markup' => $key,
      '#prefix' => '<span class="option-key">',
      '#suffix' => '</span>',
    );
    $form_entry['label'] = array(
      '#markup' => $label,
      '#prefix' => '<span class="option-label">',
      '#suffix' => '</span>',
    );

    $form_entry['#delta']  = $delta;
    $form_entry['#weight'] = $delta;
    $form_entry['_weight'] = array(
      '#type'          => 'weight',
      '#title'         => t('Weight for row @number', array('@number' => $delta + 1)),
      '#title_display' => 'invisible',
      '#delta'         => $total,
      '#default_value' => $delta,
      '#weight'        => 100,
    );
    $delta++;
  }
  unset($form['extra']['items']);

  return $form;
}

/**
 * Implements callback_form_element_validate() for currency elements.
 *
 * Ensures the currencies are ordered in the form values array like the user did
 * in the UI. Selects those currencies that where enabled by the user to be saved
 * for this component in the DB.
 *
 * @param array $element
 *   An form API element array.
 * @param array $form_state
 *   An form API array of the state of the form.
 */
function _webform_currency_edit_validate(array &$element, array &$form_state) {
  $values = &drupal_array_get_nested_value($form_state['values'], $element['#parents']);
  // Get only enabled currencies and key them by their weight for later sorting.
  $filtered = array();
  foreach ($values as $code => $value) {
    if (!empty($value['available'])) {
      $filtered[$value['_weight']] = $code;
    }
  }
  ksort($filtered);
  $values = array_combine($filtered, $filtered);
}

/**
 * Implements _webform_render_[component]().
 *
 * @param array $component
 *   A Webform component array.
 * @param array $value
 *   If editing an existing submission or resuming a draft, this will contain
 *   an array of values to be shown instead of the default in the component
 *   configuration. This value will always be an array, keyed numerically for
 *   each value saved in this field.
 * @param bool $filter
 *   Whether or not to filter the contents of descriptions and values when
 *   rendering the component. Values need to be unfiltered to be editable by
 *   Form Builder.
 *
 * @return array
 *   A renderable array
 */
function _webform_render_currency(array $component, array $value = NULL, $filter = TRUE) {
  $component['extra']['aslist'] = TRUE;
  $component['extra']['multiple'] = FALSE;
  $component['extra']['other_option'] = NULL;
  $component['extra']['options_source'] = NULL;

  // Yes, encoding the options and then parsing them again is not ideal, but
  // maintainability beats performance here.
  $options = webform_currency_options();
  if ($component['extra']['options_enabled']) {
    $options = _webform_currency_choose_options($options, $component['extra']['options_enabled']);
  }
  $component['extra']['items'] = _webform_select_options_to_text($options);
  $value = $value ? $value : key($options);
  return _webform_render_select($component, $value, $filter);
}

/**
 * Implements _webform_submit_[component]().
 *
 * @param array $component
 *   A Webform component array.
 * @param string $value
 *   The ISO code for the selected currency or '' for non mandatory fields where
 *   the user didn't choose a currency
 *
 * @return array
 *   either the unchanged ISO code or NULL if the user didn't select a currency
 */
function _webform_submit_currency(array $component, $value) {
  return empty($value) ? [] : [$value];
}

/**
 * Implements _webform_display_[component]().
 *
 * @param array $component
 *   A Webform component array.
 * @param $value
 *   An array of information containing the submission result, directly
 *   correlating to the webform_submitted_data database table schema.
 * @param string $format
 *   Either 'html' or 'text'. Defines the format that the content should be
 *   returned as. Make sure that returned content is run through check_plain()
 *   or other filtering functions when returning HTML.
 *
 * @return array
 *   A renderable element
 */
function _webform_display_currency(array $component, $value, $format = 'html') {
  return array(
    '#title'          => $component['name'],
    '#weight'         => $component['weight'],
    '#theme'          => 'webform_display_currency',
    '#theme_wrappers' => $format == 'html' ? array('webform_element') : array('webform_element_text'),
    '#post_render'    => array('webform_element_wrapper'),
    '#component'      => $component,
    '#format'         => $format,
    '#value'          => isset($value) ? $value[0] : '',
    '#translatable'   => array('title'),
  );
}

/**
 * Implements _webform_theme_[component]().
 *
 * @return array
 *   An associative array of theme hook information.
 */
function _webform_theme_currency() {
  return array(
    'currency_form' => array(
      'render element' => 'element',
    ),
    'webform_display_currency' => array(
      'render element' => 'element',
    ),
  );
}

/**
 * Implements _webform_table_[component]().
 *
 * @param array $component
 *   A Webform component array.
 * @param $value
 *   An array of information containing the submission result, directly
 *   correlating to the webform_submitted_data database schema.
 *
 * @return string
 *   Textual output formatted for human reading.
 */
function _webform_table_currency(array $component, $value) {
  return !empty($value[0]) ? check_plain($value[0]) : '';
}

/**
 * Implements _webform_csv_headers_[component]().
 *
 * @param array $component
 *   A Webform component array.
 * @param array $export_options
 *   An array of options that may configure export of this field.
 *
 * @return array
 *   An array of data to be displayed in the first three rows of a CSV file, not
 *   including either prefixed or trailing commas.
 */
function _webform_csv_headers_currency(array $component, array $export_options) {
  $component['extra']['multiple'] = FALSE;
  return _webform_csv_headers_select($component, $export_options);
}

/**
 * Implements _webform_csv_data_[component]().
 *
 * @param array $component
 *   A Webform component array.
 * @param array $export_options
 *   An array of options that may configure export of this field.
 * @param array $value
 *   An array of information containing the submission result, directly
 *   correlating to the webform_submitted_data database schema.
 *
 * @return string
 *   An array of items to be added to the CSV file. Each value within the array
 *   will be another column within the file. This function is called once for
 *   every row of data.
 */
function _webform_csv_data_currency(array $component, array $export_options, array $value) {
  return isset($value[0]) ? $value[0] : '';
}


/**********************************************************************/
/******************* FORM BUILDER INTEGRATION *************************/
/**********************************************************************/


/**
 * Implements _webform_form_builder_map_[form_builder type]().
 */
function _webform_form_builder_map_currency() {
  $map['form_builder_type'] = 'currency';
  $map['properties']        = array(
    'options_enabled' => array(
      'storage_parents' => array('extra', 'options_enabled'),
    ),
  );
  return $map;
}

/**
 * Implements _webform_form_builder_properties_[component]().
 *
 * Component specific properties.
 *
 * @see form_builder_webform_form_builder_properties().
 */
function _webform_form_builder_properties_currency() {
  return array(
    'options_enabled' => array(
      'form'   => 'webform_currency_form_builder_property_currency_form',
    ),
  );
}

/**
 * Callback set in _webform_form_builder_properties_currency().
 */
function webform_currency_form_builder_property_currency_form($component, $edit, &$form_state, $property) {
  $form['options_enabled'] = $edit['extra']['options_enabled'] + array(
    '#form_builder' => array('property_group' => 'options'),
  );
  return $form;
}
