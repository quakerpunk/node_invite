<?php

/**
 * @file
 * Node invite admin settings
 */

function node_invite_admin_settings_form() {
	$form['node_invite_node_types'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Which node types can have invites?'),
    '#description' => t('Use this to enable/disable node types that can have invites associated with them'),
    '#options' => node_type_get_names('names'),
    '#default_value' => variable_get('node_invite_node_types', array()),
    '#weight' => -10,
  );
  $form['node_invite_default'] = array(
    '#weight' => -9,
    '#title' => t('Default behavior on enabled node-types'),
    '#type' => 'select',
    '#options' => array(
      '1' => 'Enable invites',
      '0' => 'Disable invites',
    ),
    '#default_value' => variable_get('node_invite_default', NODE_INVITE_DEFAULT_FOR_ENABLED_NODES),
    '#description' => t('
      <strong>Enable Invites</strong> = All nodes of an enabled type will have a $node->link[\'\'] unless you un-check the "Enable invites for this node" box in the node/edit form<br />
      <strong>Disable Invites</strong> = To get an invite for a specific node, you will need to check the "Enable invites for this node" box in the node/edit form'),
  );
  $form['node_invite_set_email'] = array(
    '#type' => 'checkbox',
    '#title' => t('Allow user to set their own email and name'),
    '#description' => t('Leaving this unchecked will force the user to invite from their email address and username as configured in their profile.'),
    '#default_value' => variable_get('node_invite_set_email', TRUE),
  );
  $form['node_invite_default_from'] = array(
    '#type' => 'textfield',
    '#title' => t('Specify a default <em>From</em> address'),
    '#description' => t('If this field is set then all outgoing emails will have the From address set to the given value (normally something like no-reply@example.com). The <em>Reply To</em> address will be set to the users specified email address. This is recommended if you find your outgoing emails are being flagged as spam due to the sender address domain differing from the domain of the outgoing SMTP server.'),
    '#default_value' => variable_get('node_invite_default_from', ''),
  );

  $form['node_invite_hash_urls'] = array(
    '#type' => 'checkbox',
    '#title' => t('Use private URLs'),
    '#description' => t('Check this to enable hash\'d URLs that will make guessing another guests RSVP page impossible.'),
    '#default_value' => variable_get('node_invite_hash_urls', FALSE),
  );
  // @TODO: If this setting is changed then we need to clear the menu cache.
  $form['node_invite_display'] = array(
    '#type' => 'select',
    '#title' => t('How would you like the invite form to be displayed?'),
    '#options' => array(
      'link' => t('Link in the links area of a node'),
      'tab' => t('"Invite people" tab on the full node view'),
      'form' => t('Inline form on the full node view'),
    ),
    '#default_value' => variable_get('node_invite_display', 'link'),
  );

  $form['node_invite_user_invite'] = array(
    '#type' => 'fieldset',
    '#title' => t('User Invite Options'),
    '#description' => t('Setup the user/email inviting options'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $form['node_invite_user_invite']['node_invite_user_limit'] = array(
    '#type' => 'textfield',
    '#title' => t('Max users to appear in the user autocomplete dropdown.'),
    '#default_value' => variable_get('node_invite_user_limit', 10),
    '#size' => 6,
  );

  $form['node_invite_user_invite']['node_invite_disable_email'] = array(
    '#type' => 'checkbox',
    '#title' => t('Disable the email addresses invite field'),
    '#description' => t('Selecting this will mean users can only invite members of the site to nodes.'),
    '#default_value' => variable_get('node_invite_disable_email', 0),
  );
  $form['node_invite_user_invite']['node_invite_disable_users'] = array(
    '#type' => 'checkbox',
    '#title' => t('Disable the user invite field'),
    '#description' => t('Selecting this will mean users can only invite members by email address.'),
    '#default_value' => variable_get('node_invite_disable_users', 0),
  );
  $form['node_invite_user_invite']['node_invite_sync_users'] = array(
    '#type' => 'checkbox',
    '#title' => t('Keep user emails in sync with invitations'),
    '#description' => t('This will alter the invitation "sent" email address if a user changes their account email address to make sure they always stay in sync.'),
    '#default_value' => variable_get('node_invite_sync_users', 0),
  );

  if (module_exists('userreference')) {
    $form['node_invite_user_invite']['node_invite_userreference'] = array(
      '#type' => 'checkbox',
      '#title' => t('Enable user reference selection'),
      '#description' => t('This will use the user reference field method of filtering users instead of the built in method which selects from all users in the system.'),
      '#default_value' => variable_get('node_invite_userreference', 0),
    );

    $settings = array(
      'referenceable_roles' => variable_get('node_invite_referenceable_roles', array()),
      'referenceable_status' => variable_get('node_invite_referenceable_status', ''),
      'advanced_view' => variable_get('node_invite_advanced_view', '--'),
      'advanced_view_args' => variable_get('node_invite_advanced_view_args', ''),
    );

    // Copy the userreference node form settings into the node invite admin page.
    if ($ur_form = userreference_field_settings('form', $settings)) {
      $form['node_invite_user_invite']['node_invite_referenceable_roles'] = $ur_form['referenceable_roles'];
      unset($ur_form['referenceable_status']['#default_value']);
      $form['node_invite_user_invite']['node_invite_referenceable_status'] = $ur_form['referenceable_status'];
      $form['node_invite_user_invite']['advanced'] = $ur_form['advanced'];
      $form['node_invite_user_invite']['advanced']['node_invite_advanced_view'] = $ur_form['advanced']['advanced_view'];
      $form['node_invite_user_invite']['advanced']['node_invite_advanced_view_args'] = $ur_form['advanced']['advanced_view_args'];
      unset($form['node_invite_user_invite']['advanced']['advanced_view']);
      unset($form['node_invite_user_invite']['advanced']['advanced_view_args']);
    }
  }
  else {
    $form['node_invite_user_invite']['userreference_help'] = array(
      '#markup' => '<p>' . t('Enable the cck userreference field and you can use identical options to filter user selection for invitations, including role selection or views.') . '</p>',
    );
  }

  $form['node_invite_rsvp_page'] = array(
    '#type' => 'fieldset',
    '#title' => t('RSVP Page'),
    '#description' => t('Users who receive an invite, will be directed to a page asking them to accept the invite.  The parts of that page are configurable here.'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $form['node_invite_rsvp_page']['node_invite_rsvp_display'] = array(
    '#type' => 'select',
    '#title' => t('How would you like the RSVP form to be displayed?'),
    '#options' => array(
      'link' => t('Link in the links area of a node'),
      'form' => t('Inline form on the full node view'),
    ),
    '#default_value' => variable_get('node_invite_rsvp_display', 'link'),
    '#description' => t('This will only apply to nodes set up as "open invite"'),
  );

  $form['node_invite_rsvp_page']['node_invite_rsvp_show_comments'] = array(
    '#type' => 'select',
    '#options' => array(
      '1' => 'Yes',
      '0' => 'No',
    ),
    '#default_value' => variable_get('node_invite_rsvp_show_comments', '0'),
    '#title' => t('Allow invitees to leave comments?'),
    '#description' => t('This causes a text-area to show up on the RSVP form.'),
  );
  $form['node_invite_rsvp_page']['node_invite_rsvp_email'] = array(
    '#type' => 'select',
    '#title' => t('Force user to re-enter their email?'),
    '#description' => t('Optional security measure'),
    '#options' => array(
      '1' => 'Yes',
      '0' => 'No',
    ),
    '#default_value' => variable_get('node_invite_rsvp_email', '0'),
  );
  $form['node_invite_rsvp_page']['node_invite_rsvp_page_submit_pretext'] = array(
    '#type' => 'textarea',
    '#title' => t('Text to display before submit buttons'),
    '#default_value' => variable_get('node_invite_rsvp_page_submit_pretext', NODE_INVITE_RSVP_PAGE_SUBMIT_PRETEXT),
    '#description' => t('Tokens and html accepted.'),
  );
  $form['node_invite_rsvp_page']['node_invite_rsvp_page_title'] = array(
    '#type' => 'textfield',
    '#title' => t('Page Title'),
    '#default_value' => variable_get('node_invite_rsvp_page_title', NODE_INVITE_RSVP_PAGE_TITLE),
    '#description' => t('The subject is token-enabled. Please use the list below for replacement patterns.'),
  );
  $form['node_invite_rsvp_page']['node_invite_rsvp_page_intro'] = array(
    '#type' => 'textarea',
    '#title' => t('Page Intro Text'),
    '#default_value' => variable_get('node_invite_rsvp_page_intro', NODE_INVITE_RSVP_PAGE_INTRO),
    '#description' => t('This block of text will lead off the form. The subject is token-enabled. Please use the list below for replacement patterns.'),
  );

  $form['node_invite_message'] = array(
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#title' => t('Invite Message'),
  );
  $form['node_invite_message']['node_invite_message_default'] = array(
    '#type' => 'textarea',
    '#title' => t('E-Mail Sent to Invitees'),
    '#description' => t('The email we send is token-enabled. Please use the list below for replacement patterns. This email will be HTML formatted if you are using the HTMLMail module.'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#default_value' => variable_get('node_invite_message_default', NODE_INVITE_MESSAGE_DEFAULT),
    '#weight' => 0,
  );
  $form['node_invite_message']['node_invite_subject_default'] = array(
    '#type' => 'textfield',
    '#title' => t('Subject for emails'),
    '#description' => t('The subject is token-enabled. Please use the list below for replacement patterns.'),
    '#default_value' => variable_get('node_invite_subject_default', NODE_INVITE_SUBJECT_DEFAULT),
    '#weight' => -1,
  );
  $form['node_invite_tokens'] = array(
    '#type'  => 'fieldset',
    '#title' => t('Token Help'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#description' => "<p>Not all tokens will be available. The following lists all tokens known to Drupal.  The list of <i>available</i> tokens will depend on the type of node to which you are inviting people.</p>" . theme('token_tree', array('token_types' => array('node_invite', 'node', 'user'))),
  );

  return system_settings_form($form);
}

function node_invite_admin_settings_form_validate($form, &$form_state) {
  // Can't disable both the email and user fields at the same time
  if ($form_state['values']['node_invite_disable_email'] &&
    $form_state['values']['node_invite_disable_users']) {
    form_set_error('node_invite_disable_email', t('You cannot disable both the email and user invite fields or your users will not be able to invite anyone.'));
  }
}

function node_invite_notifications_form() {
  $form['node_invite_intro'] = array(
    '#markup' => '<p>This form allows you to manage notifications set out by the node_invite module</p>'
  );
  $form['node_invite_nots_rsvps'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Who should we notify when someone RSVPs?'),
    '#options' => array(
      'invitee' => '<strong>Invitee:</strong> Upon selection of any option, the invitee should receive an email confirming their selection (regardless of Yes/No/Maybe)',
      'inviter' => '<strong>Inviter:</strong> The person who sent the original invite should receive an email notifying them that someone answer their invite (regardless of Yes/No/Maybe)',
    ),
    '#default_value' => variable_get('node_invite_nots_rsvps', array('inviter', 'invitee')),
  );
  $form['node_invite_nots_admin'] = array(
    '#type' => 'radios',
    '#title' => t('When should we notify the site admin of an invite response?'),
    '#options' => array(
      'site_admin_never' => t('Never notify the site admin of an invite response.'),
      'site_admin_yes' => t('Site Admin User should receive an email whenever someone ACCEPTS an invite'),
      'site_admin_all' => t('Site Admin User should receive an email whenever someone responds (with any answer) to an invite'),
    ),
    '#default_value' => variable_get('node_invite_nots_admin', 'site_admin_all'),
  );
  $form['node_invite_nots_sep'] = array(
    '#markup' => '<h1>Messages</h1>',
  );
  $form['node_invite_fs_site_admin_yes'] = array(
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#title' => t('Site Admin User should receive an email whenever someone ACCEPTS an invite'),
  );
  $form['node_invite_fs_site_admin_yes']['node_invite_notify_site_admin_yes_subject'] = array(
    '#type' => 'textfield',
    '#title' => t('Subject'),
    '#description' => t('Token enabled'),
    '#default_value' => variable_get('node_invite_notify_site_admin_yes_subject', NODE_INVITE_NOTIFY_SITE_ADMIN_YES_SUBJECT),
  );
  $form['node_invite_fs_site_admin_yes']['node_invite_notify_site_admin_yes'] = array(
    '#type' => 'textarea',
    '#description' => t('Token enabled'),
    '#title' => t('Notification Message for this situation'),
    '#default_value' => variable_get('node_invite_notify_site_admin_yes', NODE_INVITE_NOTIFY_SITE_ADMIN_YES),
  );
  $form['node_invite_fs_site_admin_all'] = array(
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#title' => t('Site Admin User should receive an email whenever someone responds (with any answer) to an invite'),
  );
  $form['node_invite_fs_site_admin_all']['node_invite_notify_site_admin_all_subject'] = array(
    '#type' => 'textfield',
    '#title' => t('Subject'),
    '#description' => t('Token enabled'),
    '#default_value' => variable_get('node_invite_notify_site_admin_all_subject', NODE_INVITE_NOTIFY_SITE_ADMIN_ALL_SUBJECT),
  );
  $form['node_invite_fs_site_admin_all']['node_invite_notify_site_admin_all'] = array(
    '#type' => 'textarea',
    '#description' => t('Token enabled'),
    '#title' => t('Notification Message for this situation'),
    '#default_value' => variable_get('node_invite_notify_site_admin_all', NODE_INVITE_NOTIFY_SITE_ADMIN_ALL),
  );
  $form['node_invite_fs_invitee_confirm'] = array(
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#title' => t('Upon selection of any option, the invitee should receive an email confirming their selection (regardless of Yes/No/Maybe)'),
  );
  $form['node_invite_fs_invitee_confirm']['node_invite_notify_invitee_confirm_subject'] = array(
    '#type' => 'textfield',
    '#title' => t('Subject'),
    '#description' => t('Token enabled'),
    '#default_value' => variable_get('node_invite_notify_invitee_confirm_subject', NODE_INVITE_NOTIFY_INVITEE_CONFIRM_SUBJECT),
  );
  $form['node_invite_fs_invitee_confirm']['node_invite_notify_invitee_confirm'] = array(
    '#type' => 'textarea',
    '#description' => t('Token enabled'),
    '#title' => t('Notification Message for this situation'),
    '#default_value' => variable_get('node_invite_notify_invitee_confirm', NODE_INVITE_NOTIFY_INVITEE_CONFIRM),
  );
  $form['node_invite_fs_inviter_notify'] = array(
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#title' => t('The person who sent the original invite should receive an email notifying them that someone answer their invite (regardless of Yes/No/Maybe)'),
  );
  $form['node_invite_fs_inviter_notify']['node_invite_notify_inviter_notify_subject'] = array(
    '#type' => 'textfield',
    '#title' => t('Subject'),
    '#description' => t('Token enabled'),
    '#default_value' => variable_get('node_invite_notify_inviter_notify_subject', NODE_INVITE_NOTIFY_INVITER_NOTIFY_SUBJECT),
  );
  $form['node_invite_fs_inviter_notify']['node_invite_notify_inviter_notify'] = array(
    '#type' => 'textarea',
    '#description' => t('Token enabled'),
    '#title' => t('Notification Message for this situation'),
    '#default_value' => variable_get('node_invite_notify_inviter_notify', NODE_INVITE_NOTIFY_INVITER_NOTIFY),
  );
  $form['node_invite_tokens'] = array(
    '#type'  => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#description' => theme('token_help', 'node'),
    '#title' => t('Available Tokens'),
  );

  return system_settings_form($form);
}

function node_invite_manage_page() {
  $output = '';
  $output .= '<p>' . t("This form allows you to manage those invites you've sent out.") . '</p>';
  $output .= node_invite_list_invites();
  return $output;
}

/**
 * Revoke the invite.
 */
function node_invite_revoke($form, &$form_state, $invite) {
  // ask for confirmation that they really want to revoke the invite.

  $form['invite'] = array(
    '#type' => 'value',
    '#value' => $invite,
  );

  $form['#redirect'] = $_GET['destination'];

  return confirm_form($form, 'Are you sure you wish to revoke this invite?', $_GET['destination'] );
}

function node_invite_revoke_submit($form, &$form_state) {
  $invite = $form_state['values']['invite'];
  node_invite_save($invite, array('status' => NODE_INVITE_REVOKED));
}

function node_invite_reinstate($invite) {
  node_invite_save($invite, array('status' => NODE_INVITE_SENT));
  drupal_goto($_GET['destination']);
}

/**
 * Resend the invite.
 */
function node_invite_resend($form, &$form_state, $invite) {
	module_load_include('inc', 'node_invite', 'node_invite.send');

  // Setup the node send form
  $resend_form = drupal_get_form('node_invite_send', node_load($invite->nid));

  $resend_form['invite']['your_name']['#default_value'] = $invite->inviter_custom_name ? $invite->inviter_custom_name : $invite->name;
  $resend_form['invite']['your_email']['#default_value'] = $invite->inviter_custom_email ? $invite->inviter_custom_email : $invite->mail;
  $resend_form['invite']['personal_message']['message']['#default_value'] = $invite->personal_message;
  if ($invite->uid_invitee && !variable_get('node_invite_disable_users', FALSE)) {
    $resend_form['invite']['users']['#value'] = $account->name;
  }
  else {
    $resend_form['invite']['emails']['#value'] = $invite->email_invitee;
  }
  // Don't let the user alter these values on the resend form.
  $resend_form['invite']['users']['#type'] = 'value';
  $resend_form['invite']['emails']['#type'] = 'value';

  $resend_form['emails_markup']['submit']['#value'] = t('Resend Invite');

  // ask for confirmation that they really want to resend the invite.
  $resend_form['emails_markup']['invite'] = array(
    '#type' => 'value',
    '#value' => $invite,
  );

  if (!empty($_GET['destination'])) {
    $resend_form['#redirect'] = $_GET['destination'];
  }
  else {
    $resend_form['#redirect'] = 'node/' . $invite->nid . '/manage_invites';
  }

  $resend_form['#validate'] = array('node_invite_send_validate');
  return $resend_form;
}

function node_invite_resend_submit($form, &$form_state) {
  $invite = $form_state['values']['invite'];
  if (!isset($invite->iid)) {
    drupal_set_message('Unable to fetch information about this invite. Resend failed.', 'error');
    return;
  }

  $params = array(
    'status' => NODE_INVITE_SENT,
    'acted_upon' => NULL,
    'inviter_custom_name' => $form_state['values']['your_name'],
    'inviter_custom_email' => $form_state['values']['your_email'],
    'personal_message' => $form_state['values']['message'],
  );
  $invite = node_invite_save($invite, $params);
  $node = $form_state['values']['node'];
  node_invite_specific_info($invite, $node);
  if (variable_get('node_invite_set_email', TRUE)) {
    $node->invite_specific_info['inviter-name'] = $form_state['values']['your_name'];
    $node->invite_specific_info['inviter-mail'] = $form_state['values']['your_email'];
  }
  node_invite_send_mail($invite, $node);
  drupal_set_message('Invite resent to ' . $invite->email_invitee);
  unset($form_state['storage']);
}
