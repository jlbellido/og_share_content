<?php

/**
 * Form to share our content.
 */
function og_share_share_form($form, $form_state) {
  $form = array();

  // Get the bundles that are group content:
  $my_groups_options = _og_share_get_user_groups_options($form_state['build_info']['args'][0]);
  if (!empty($my_groups_options)) {
    $form['my-groups'] = array(
      '#title' => t('Select the content types that will be allowed to be shared'),
      '#type' => 'checkboxes',
      '#required' => TRUE,
      '#options' => _og_share_get_user_groups_options($form_state['build_info']['args'][0]),
    );

    $form['actions'] = array(
      '#type' => 'container',
      '#attributes' => array(
        'class' => array('form-actions')
      )
    );

    $form['actions']['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Share'),
    );
  }
  else {
    // The conten has been shared in user's communities.
    $form['message'] = array(
      '#prefix' => '<span class="og_sahre_message">',
      '#type' => 'markup',
      '#markup' => t('Sorry, this content is already shared in all your commnuties'),
      '#suffix' => '</span>',
    );
  }

  return $form;
}

/**
 * Implements FORM_ID_submit().
 */
function og_share_share_form_submit($form, $form_state) {
  $node = NULL;
  // Clean teh options not selected:
  $groups_values = array_filter($form_state['values']['my-groups']);
  $args = $form_state['build_info']['args'];
  if (is_object($args[0]) && isset($args[0]->nid)) {
    $node = $args[0];
  }

  // Allow to others to create new nodes: an Share it
  drupal_alter('og_share_share', $node, $form_state['build_info']['args']);

  if (isset($node)) {
    // Save the new og_group_refs
    $w_node = entity_metadata_wrapper('node', $node);
    $current_og_refs = $w_node->og_group_ref->raw();
    if (!empty($current_og_refs)) {
      $current_og_refs = array_merge($current_og_refs, $groups_values);
    }
    else {
      $current_og_refs = $groups_values;
    }
    $w_node->og_group_ref->set($current_og_refs);
    $w_node->save();

    #Invoke hook with shared networks and node
    module_invoke_all('og_shared_content', $groups_values, $node);

    drupal_set_message(t('@title has been shared', array('@title' => $w_node->get('title')->value())));
  }
}
