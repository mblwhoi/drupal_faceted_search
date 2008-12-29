<?php
// $Id$

/**
 * @file
 * Administrative page callbacks for the Faceted Search module.
 */

/**
 * List all environments and provide links to operations on those.
 */
function faceted_search_list_page() {
  $env_ids = faceted_search_get_env_ids();
  if (count($env_ids)) {
    $headers = array(t('Environment'), t('Description'), t('Operations'));
    $items = array();
    foreach ($env_ids as $env_id) {
      $links = array(
        array('title' => t('Edit'), 'href' => 'admin/settings/faceted_search/'. $env_id),
        array('title' => t('Delete'), 'href' => 'admin/settings/faceted_search/delete/'. $env_id),
      );
      $name = faceted_search_variable_get($env_id, 'name', $env_id);
      $path = faceted_search_variable_get($env_id, 'base_path', NULL);
      $env = isset($path) ? l($name, $path) : $name; // Validate the path, because it is a faceted_search_ui thing, normally unknown by faceted_search.
      $description = faceted_search_variable_get($env_id, 'description', '');
      $items[$name] = array($env, filter_xss_admin($description), theme('links', $links));
    }
    ksort($items); // Sort items by name.
    $output = '<p>'. t('This page shows all of the faceted search environments that are currently defined.') .'</p>';
    $output .= theme('table', $headers, $items);
  }
  else {
    $output = '<p>'. t('No faceted search environments have currently been defined.') .'</p>';
  }
  return $output;
}

/**
 * Add a new environment.
 */
function faceted_search_add_page() {
  drupal_set_title(t('Add a faceted search environment'));
  return drupal_get_form('faceted_search_edit_form', NULL);
}

/**
 * Delete an environment.
 */
function faceted_search_delete_form($form_state, $env_id) {
  $form['env_id'] = array(
    '#type' => 'value',
    '#value' => $env_id,
  );
  $form = confirm_form(
    $form,
    t('Are you sure you want to delete %env?', array('%env' => faceted_search_variable_get($env_id, 'name', $env_id))),
    'admin/settings/faceted_search',
    t('This action cannot be undone.'),
    t('Delete'),
    t('Cancel')
  );
  return $form;
}

/**
 * Handle the submit button to delete an environment.
 */
function faceted_search_delete_form_submit($form, &$form_state) {
  $env = faceted_search_variable_get($form_state['values']['env_id'], 'name', $form_state['values']['env_id']);

  faceted_search_variable_del_all($form_state['values']['env_id']);
  db_query('DELETE FROM {faceted_search_filters} WHERE env_id = %d', $form_state['values']['env_id']);
  menu_rebuild();

  drupal_set_message(t('Faceted search environment %env deleted.', array('%env' => $env)));

  $form_state['redirect'] = 'admin/settings/faceted_search';
}

/**
 * Form for editing an environment.
 *
 * @param $env_id
 *   The id of the environment to edit, or null if adding a new one.
 */
function faceted_search_edit_form($form_state, $env_id = NULL) {
  drupal_set_title(t('Faceted search environment: @env', array('@env' => faceted_search_variable_get($env_id, 'name', $env_id))));
  if (isset($env_id)) {
    $form['env_id'] = array(
      '#type' => 'value',
      '#value' => $env_id,
    );
  }

  // Basic information section.
  $form['info'] = array(
    '#type' => 'fieldset',
    '#title' => t('Basic information'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['info']['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Name'),
    '#maxlength' => 32,
    '#size' => 32,
    '#required' => TRUE,
    '#default_value' => isset($env_id) ? faceted_search_variable_get($env_id, 'name', $env_id) : '',
    '#description' => t('A unique name to help site administrators identify the environment. Only alphanumeric and the underscore character are allowed here.'),
  );
  $form['info']['title'] = array(
    '#type' => 'textfield',
    '#title' => t('Title'),
    '#maxlength' => 128,
    '#size' => 32,
    '#required' => TRUE,
    '#default_value' => isset($env_id) ? faceted_search_variable_get($env_id, 'title', $env_id) : t('Search'),
    '#description' => t("The title shown to users on the environment's pages."),
  );
  $form['info']['description'] = array(
    '#type' => 'textfield',
    '#title' => t('Description'),
    '#maxlength' => 255,
    '#size' => 60,
    '#default_value' => isset($env_id) ? faceted_search_variable_get($env_id, 'description', '') : '',
    '#description' => t('The description of the environment for site administrators.'),
  );
  $types = array();
  foreach (array_keys(node_get_types('names')) as $type) {
    $types[$type] = $type;
  }
  $form['info']['types'] = array(
    '#title' => t('Content types'),
    '#type' => 'checkboxes',
    '#options' => node_get_types('names'),
    '#description' => t('Only the checked types will appear in this search environment. If none is checked, all types will be allowed.'),
    '#default_value' => array_intersect($types, array_filter(faceted_search_variable_get($env_id, 'types', array()))),
  );
  $form['info']['ignore_status'] = array(
    '#type' => 'checkbox',
    '#title' => t('Let unpublished nodes appear in search results'),
    '#default_value' => isset($env_id) ? faceted_search_variable_get($env_id, 'ignore_status', FALSE) : FALSE,
    '#description' => t("When this is enabled, unpublished nodes are allowed to appear in search results to users with the 'administer nodes' permission. Note that unpublished nodes are never indexed by Drupal core, so they will never appear in keyword search results although they <em>will</em> appear in guided search results."),
    '#weight' => 1,
  );

  // Facets section.
  $form['facets'] = array(
    '#type' => 'fieldset',
    '#title' => t('Facets'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#description' => t('<p>Facets provide categories for users to browse and refine their search.</p><p>Check the facets you wish to expose to users. In listings, the heavier facets will sink and the lighter facets will be positioned nearer the top.</p><p>The Sort criteria applies to categories listings only, and the Categories limit applies to category listings in the Guided search only. For performance reasons, it is advisable to select some limit (users will still be able to get the full listing of categories by following a <em>more</em> link).</p>'),
  );
  $form['facets']['facets'] = array(
    '#theme' => 'faceted_search_facets_settings',
    '#tree' => TRUE,
  );

  // Gather every possible facet.
  $all_settings = faceted_search_load_filter_settings($env_id, TRUE);
  $facets = array();
  foreach (module_implements('faceted_search_collect') as $module) {
    $hook = $module .'_faceted_search_collect';
    $hook($facets, 'facets', $env_id, NULL);
  }

  // Prepare facets for use, assigning them their settings and sorting them.
  faceted_search_prepare_filters($facets, $all_settings);

  // Add the facets section's content to the form.
  _faceted_search_facet_settings_form($form['facets']['facets'], $facets);

  // Keyword search section.
  $form['keyword'] = array(
    '#type' => 'fieldset',
    '#title' => t('Keyword search'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['keyword']['fields'] = array(
    '#type' => 'fieldset',
    '#title' => t('Fields'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#description' => t('<p>Users may restrict keyword search by field. <em>Anywhere</em> lets them search for keywords in the full nodes.</p><p>Check the fields you wish to expose to users for keyword search. In listings, the heavier fields will sink and the lighter fields will be positioned nearer the top.</p>'),
  );
  $form['keyword']['fields']['keyword_filters'] = array(
    '#theme' => 'faceted_search_keyword_filters_settings',
    '#tree' => TRUE,
  );

  // Gather keyword filters.
  $keyword_filters = array();
  foreach (module_implements('faceted_search_collect') as $module) {
    $hook = $module .'_faceted_search_collect';
    $hook($keyword_filters, 'keyword filters', $env_id, NULL);
  }
  // Gather the node keyword filter. This is the default, always-enabled keyword
  // filter that allows searching in the full node index.
  faceted_search_collect_node_keyword_filters($keyword_filters, 'keyword filters', $env_id);

  // Prepare facets for use, assigning them their settings and sorting them.
  faceted_search_prepare_filters($keyword_filters, $all_settings);

  // Add the facets section's content to the form.
  _faceted_search_filter_settings_form($form['keyword']['fields']['keyword_filters'], $keyword_filters);

  // Buttons.
  $form['buttons']['#weight'] = 1000; // Ensure buttons remain at the bottom when form is altered.
  $form['buttons']['save'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );
  $form['buttons']['save_and_edit'] = array(
    '#type' => 'submit',
    '#value' => t('Save and edit'),
  );

  return $form;
}

/**
 * Validation callback for the environment edit form.
 */
function faceted_search_edit_form_validate($form, &$form_state) {
  // Name must be alphanumeric or underscores, no other punctuation.
  if (preg_match('/[^a-zA-Z0-9_]/', $form_state['values']['name'])) {
    form_set_error('name', t('Name must be alphanumeric or underscores only.'));
  }

  // Name must be unique.
  if (db_result(db_query("SELECT COUNT(*) FROM {faceted_search_variables} WHERE name = 'name' AND value = '%s' AND env_id <> %d", serialize($form_state['values']['name']), isset($form_state['values']['env_id']) ? $form_state['values']['env_id'] : 0)) > 0) {
    form_set_error('name', t('Name %name already in use.', array('%name' => $form_state['values']['name'])));
  }

  // Weights must be numeric.
  if ($form_state['values']['facets']) {
    foreach ($form_state['values']['facets'] as $key => $settings) {
      if (!is_numeric($settings['weight'])) {
        form_set_error("facets][$key][weight", t('Weight must be a numeric value.'));
      }
    }
  }
  foreach ($form_state['values']['keyword_filters'] as $key => $settings) {
    if (!is_numeric($settings['weight'])) {
      form_set_error("keyword_filters][$key][weight", t('Weight must be a numeric value.'));
    }
  }
}

/**
 * Form callback for saving environment data.
 */
function faceted_search_edit_form_submit($form, &$form_state) {
  if (isset($form_state['values']['env_id'])) {
    $env_id = $form_state['values']['env_id'];
  }
  else {
    $ids = faceted_search_get_env_ids();
    sort($ids);
    $env_id = $ids[0] + 1;
    //$env_id = db_last_insert_id('faceted_search_filters','env_id'); // Adding new environment.
  }

  // Exclude elements that should not be saved as environment variables.
  $exclude = array('save', 'save_and_edit', 'form_id', 'op', 'form_token', 'env_id', 'facets', 'keyword_filters');

  // Save form values as environment variables (except those excluded).
  foreach ($form_state['values'] as $key => $value) {
    if (!in_array($key, $exclude)) {
      faceted_search_variable_set($env_id, $key, $value);
    }
  }

  // Save filter settings.
  if ($form_state['values']['facets']) {
    faceted_search_save_filter_settings($env_id, array_merge($form_state['values']['facets'], $form_state['values']['keyword_filters']));
  }

  // Rebuild the menus, if only for updating titles.
  menu_rebuild();

  drupal_set_message(t('The faceted search environment %env has been saved.', array('%env' => $form_state['values']['name'])));

  if ($form_state['values']['op'] == t('Save')) {
    $form_state['redirect'] = 'admin/settings/faceted_search';
  }
  else {
    $form_state['redirect'] = 'admin/settings/faceted_search/'. $env_id;
  }
}

