<?php
/**
 * @file
 * Hook implementations for the Ubercart Addresses module.
 */

/**
 * Implements hook_uc_addresses_fields().
 *
 * Register all address fields used by Ubercart Addresses.
 *
 * @return array
 */
function uc_extra_fields_pane_uc_addresses_fields() {
  try {
    $data = array();
    $fields = UCXF_FieldList::getAllAddressFields();

    foreach ($fields as $field) {
      $data[$field->db_name] = array(
        'title' => $field->output('label'),
        'handler' => 'UcAddressesUCXFHandler',
        'display_settings' => array(
          'default' => TRUE,
          'address_view' => $field->may_display('uc_addresses'),
          'checkout_review' => $field->may_display('review'),
          'order_view' => $field->may_display('order'),
        ),
      );
    }

    return $data;
  }
  catch (UCXF_Exception $e) {
    $e->printMessage();
    $e->logError();
  }
}

/**
 * Implements hook_uc_addresses_field_handlers().
 *
 * Register form fields and their definitions.
 *
 * @return array
 */
function uc_extra_fields_pane_uc_addresses_field_handlers() {
  $path = drupal_get_path('module', 'uc_extra_fields_pane') . '/includes';

  $info = array();
  $info['UcAddressesUCXFHandler'] = array(
    'hidden' => FALSE,
    'handler' => array(
      'parent' => 'UcAddressesFieldHandler',
      'class' => 'UcAddressesUCXFHandler',
      'file' => 'uc_addresses.handlers.inc',
      'path' => $path,
    ),
  );

  return $info;
}

/**
 * Implements hook_uc_addresses_load_address().
 *
 * @param UcAddressesAddress $address
 *   The address object.
 * @param object $obj
 *   The fetched database record.
 *
 * @return void
 */
function uc_extra_fields_pane_uc_addresses_address_load($address, $obj) {
  // Load values
  $value_list = uc_extra_fields_pane_value_list_load($address->getId(), UCXF_Value::UCXF_VALUE_ADDRESS);

  foreach ($value_list as $oValue) {
    $address->setField($oValue->db_name, $oValue->getValue());
  }
}

/**
 * Implements hook_uc_addresses_insert_address().
 *
 * @param UcAddressesAddress $address
 *   The address object.
 *
 * @return void
 */
function uc_extra_fields_pane_uc_addresses_address_insert($address) {
  // Load fields for uc_addresses.
  $fields = UCXF_FieldList::getAllAddressFields();

  foreach ($fields as $field) {
    $value = $address->getField($field->db_name);
    uc_extra_fields_pane_value_save(
      array(
        'element_id' => $address->getId(),
        'element_type' => UCXF_Value::UCXF_VALUE_ADDRESS,
        'field_id' => $field->field_id,
        'value' => $address->getField($field->db_name),
      )
    );
  }
}

/**
 * Implements hook_uc_addresses_update_address().
 *
 * @param UcAddressesAddress $address
 *   The address object
 * @return void
 */
function uc_extra_fields_pane_uc_addresses_address_update($address) {
  // Do the same as when an address is inserted.
  uc_extra_fields_pane_uc_addresses_address_insert($address);
}

/**
 * Implements hook_uc_addresses_delete_address().
 *
 * @param UcAddressesAddress $address
 *   The address object.
 *
 * @return void
 */
function uc_extra_fields_pane_uc_addresses_address_delete($address) {
  // Delete all values associated with this address.
  db_delete('uc_extra_fields_values')
    ->condition('element_id', $address->getId())
    ->condition('element_type', UCXF_Value::UCXF_VALUE_ADDRESS)
    ->execute();
}

/**
 * Implements hook_uc_addresses_preprocess_address_alter().
 *
 * Don't display address fields in panes if is set they shouldn't
 * appear in that pane. (UCXF_AddressField->pane_types setting)
 *
 * @param array $fields
 *   An array containing the field data.
 * @param UcAddressesAddress $address
 *   The address object.
 * @param string $context
 *   The context in which the fields will be displayed.
 *
 * @return void
 */
function uc_extra_fields_pane_uc_addresses_preprocess_address_alter(&$fields, $address, $context) {
  if (!empty($address->ucxf_pane_type)) {
    // Check which fields may be displayed in one pane only.
    // Remove fields that may not be shown in the active pane.
    $ucxf_fields = UCXF_FieldList::getAllAddressFields();
    foreach ($ucxf_fields as $ucxf_field) {
      if (isset($fields[$ucxf_field->db_name]) && !$ucxf_field->in_pane($address->ucxf_pane_type)) {
        unset($fields[$ucxf_field->db_name]);
      }
    }
  }
}

/**
 * Implements hook_uc_addresses_select_addresses().
 *
 * Provide addresses from previous orders combined with extra address fields.
 *
 * @param int $uid
 *   The user ID to select addresses for.
 * @param string $context
 *   The context in which the addresses are used:
 *   - checkout_form
 *   - order_form
 * @param string $type
 *   The type of address to select addresses for (shipping or billing).
 *
 * @return array
 *   An array of address arrays.
 *
 * @todo Implement with hook_query_alter() instead?
 */
function uc_extra_fields_pane_uc_addresses_select_addresses($uid, $context, $type) {
  return;
  /*
  // Check if user has any saved addresses
  $addresses = uc_addresses_uc_addresses_select_addresses($uid, $context, $type);
  foreach ($addresses as $address) {
    if ($address instanceof UcAddressesAddress) {
      if (!$address->isNew()) {
        // Don't provide addresses if the list contains saved addresses.
        return array();
      }
    }
  }
  // Return addresses from previous orders combined with extra address fields.
  module_load_include('inc', 'uc_extra_fields_pane', 'includes/address_select');
  $addresses = uc_extra_fields_pane_get_addresses($uid, $type);
  return $addresses;
  */
}

/**
 * Implements hook_uc_addresses_select_addresses_alter().
 *
 * Remove all uc_order addresses.
 *
 * @param array $addresses
 *   An array of UcAddressesAddress instances.
 * @param int $uid
 *   The user ID to select addresses for.
 * @param string $context
 *   The context in which the addresses are used:
 *   - checkout_form
 *   - order_form
 * @param string $type
 *   The type of address to select addresses for (shipping or billing).
 *
 * @return void
 *
 * @todo Implement?
 */
function uc_extra_fields_pane_uc_addresses_select_addresses_alter(&$addresses, $uid, $context, $type = 'billing') {
  return;
  /*
  foreach ($addresses as $index => $address) {
    if ($address->module == 'uc_order') {
      unset($addresses[$index]);
    }
  }
  */
}
