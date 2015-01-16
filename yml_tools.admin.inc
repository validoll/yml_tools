<?php

/**
 * @file
 * Administration page callbacks for the YML Tools module.
 */

/**
 * Administration page callback.
 */
function yml_tools_admin_settings() {
  $form = array();
  $currency_config_url = '';
  $image_field_markup = '';
  $image_field_value = '';
  $strore_creator = new YmlStore();
  $store = $strore_creator->createStore();
  $instances = field_info_instances('node');
  $term_fields = array();
  $descr_fields = array();
  $ctypes = $store->getContentTypes();
  // We use first ctype to get available fields for now.
  $i = 0;
  $ctype_first = $ctypes[$i];

  while(!isset($ctype_first)) {
    $ctype_first = $ctypes[$i++];
  }

  $instance = $instances[$ctype_first];

  $ctypes = array_combine($ctypes, $ctypes);

  $store_type = $store->getType();
  switch ($store_type) {
    case YML_TOOLS_STORE_TYPE_UBERCART:
      $currency_config_url = 'admin/store/settings/store/edit/format';
      $image_field_markup = variable_get('uc_image_' . $ctype_first, 'uc_product_image') .
        ' (' . l(t("Change"), 'admin/structure/types/manage/' . $ctype_first . '/edit') . ')';
      $image_field_value = variable_get('uc_image_' . $ctype_first, 'uc_product_image');
      break;

    case YML_TOOLS_STORE_TYPE_COMMERCE:
      $currency_config_url = 'admin/commerce/config/currency';
      // TODO: fix multiple image fields for commerce
      $image_field_markup = 'field_images' .
        ' (' . l(t("Change"), 'admin/commerce/config/product-variation-types/' . $ctype_first . '/edit') . ')';
      $image_field_value = 'field_images';
      break;

    default:
      break;
  }

  $form['yml_tools_types'] = array(
    '#required' => TRUE,
    '#type' => 'checkboxes',
    '#options' => $ctypes,
    '#default_value' => variable_get('yml_tools_types', array('product' => 'product')),
    '#title' => t('Select node types for export'),
  );

  foreach ($instance as $field_name => $field) {
    $info = field_info_field($field_name);
    if ($info['type'] == 'taxonomy_term_reference') {
      $term_fields[] = $field_name;
    }

    if ($info['module'] == 'text') {
      $descr_fields[] = $field_name;
    }
  }

  if (count($term_fields) == 0) {
    drupal_set_message(t("No term fields attached to product node! Export can't work properly. Please create at least one taxonomy vocabulary and use it for your products."), 'warning');
  }
  else {
    $term_fields = array_combine($term_fields, $term_fields);
  }

  if (count($descr_fields) == 0) {
    drupal_set_message(t("No text fields attached to product node! Export can't work properly. Please create at least one text field and use it for your products."), 'warning');
  }
  else {
    $descr_fields = array_combine($descr_fields, $descr_fields);
  }

  $form['yml_tools_term_field'] = array(
    '#required' => TRUE,
    '#type' => 'select',
    '#empty_value' => '',
    '#options' => $term_fields,
    '#default_value' => variable_get('yml_tools_term_field', ''),
    '#description' => t('Select product term field where primary product categories are stored.'),
    '#title' => t('Category field'),
  );

  $form['yml_tools_descr_field'] = array(
    '#required' => TRUE,
    '#type' => 'select',
    '#empty_value' => '',
    '#options' => $descr_fields,
    '#default_value' => variable_get('yml_tools_descr_field', ''),
    '#description' => t('Select text field which will be used as product description.'),
    '#title' => t('Description field'),
  );

  $form['yml_tools_delivery'] = array(
    '#type' => 'select',
    '#options' => array('true' => t("true"), 'false' => t("false")),
    '#title' => t('Select if delivery is enabled'),
    '#default_value' => variable_get('yml_tools_delivery', 'true'),
    '#description' => t('Yandex.Market has "delivery" field. Select if it is enabled.'),
  );

  $form['yml_tools_currency'] = array(
    '#type' => 'item',
    '#title' => t('Ubercart currency'),
    '#markup' => $store->getCurrency() . ' (' . l(t("Change"), $currency_config_url) . ')',
    '#description' => t("Should be 'RUR' for Russia"),
  );

  $form['image_field'] = array(
    '#type' => 'item',
    '#title' => t('Image field'),
    '#markup' => $image_field_markup,
    '#description' => t("This field will be used for images in export."),
  );

  $form['yml_tools_image_field'] = array(
    '#type' => 'value',
    '#value' => $image_field_value,
  );

  $url = url(YML_TOOLS_XML_URL, array('absolute' => TRUE));
  $form['generated_url'] = array(
    '#type' => 'item',
    '#title' => t("Generated file"),
    '#markup' => $url . ' (' . l(t("View"), $url) . ')',
    '#description' => t("Use this url in Yandex.Market"),
  );

  $form['#submit'][] = 'yml_tools_admin_settings_submit';

  return system_settings_form($form);
}

/**
 * Processes ml_tools_admin_settings form.
 */
function yml_tools_admin_settings_submit($form, $form_state) {
  $term_field = field_info_field($form_state['values']['yml_tools_term_field']);

  $vocab = taxonomy_vocabulary_machine_name_load($term_field['settings']['allowed_values'][0]['vocabulary']);
  variable_set('yml_tools_vid', $vocab->vid);
}
