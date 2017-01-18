<?php
/**
 * @file
 * Provides the invite add form.
 */

function node_invite_add_form($form, &$form_state, $node, $inline = FALSE) {
  global $user;

  // Get the node content type info
  $node_types = node_type_get_types();
  $node_names = node_type_get_names();

  if ($inline == TRUE) {
    $form['invite'] = array(
      '#type' => 'fieldset',
      '#title' => t('Invite people to this @type', array('@type' => $node->type)),
      '#description' => '',
      '#collapsed' => TRUE,
      '#collapsible' => TRUE,
    );

  }

  $form['invite'] = array(
    '#type' => 'fieldset',
  	'#title' => t('Invite people to this @type', array('@type' => $node_names[$node->type])),
    '#description' => '<p class="instructions">' . t('The first step to inviting people is to create the invitation below.  You can either enter the names and email addresses for the invitees in the <strong>Enter Invitation Recipients</strong> field, or you can upload a csv file in the <strong>Upload Email Addresses</strong> field.') . '</p>',
    '#collapsed' => TRUE,
    '#collapsible' => FALSE,
  );

  if (variable_get('node_invite_set_email', TRUE) || $user->uid == 0) {
    $form['invite']['your_name'] = array(
      '#title' => t('Your Name'),
      '#type' => 'textfield',
      '#default_value' => $user->name,
      '#weight' => 0,
    );
    $form['invite']['your_email'] = array(
      '#title' => t('Your E-mail Address'),
      '#type' => 'textfield',
      '#default_value' => $user->mail,
      '#weight' => 1,
    );
  }
  else {
    $form['invite']['your_name'] = array(
      '#type' => 'value',
      '#value' => $user->name,
    );
    $form['invite']['your_email'] = array(
      '#type' => 'value',
      '#value' => $user->mail,
    );
  }

  $form['invite']['personal_message'] = array(
      '#weight' => 2,
  );

  $form['invite']['personal_message']['message'] = array(
    '#type' => 'textarea',
    '#title' => t('Personal Message'),
    '#description' => t('Your personal message is going to be HTML formatted if you are using the HTMLMail module.  In addition, the personal message can take advantage of tokens.  See Token Help below.'),
  );

  $form['invite']['csv_upload'] = array(
    '#name' => 'files[csv_upload]',
    '#type' => 'file',
    '#title' => t('Upload Email Addresses'),
    '#size' => 40,
    '#description' => t('Select the CSV file to be imported. You can create a CSV file from Excel by choosing "Save As..." and selecting "Comma Separated Values (.csv) from the Format dropdown.'),
    '#weight' => 3,
  );

  if (!variable_get('node_invite_disable_users', 0)) {
    $form['invite']['users'] = array(
      '#type' => 'textfield',
      '#title' => t('Users to invite'),
      '#autocomplete_path' => 'node_invite/user_autocomplete',
      '#description' => t('Enter the list of users you would like to invite, separated by commas'),
      '#weight' => 4,
    );
  }

  if (!variable_get('node_invite_disable_email', 0)) {
    $form['invite']['emails'] = array(
      '#title'       => t('Enter Invitation Recipients'),
      '#type'        => 'textarea',
      '#description' => t('One recipient per line please.  Recipients can be entered as Name|Email or just an email address.'),
      '#weight' => 5,
    );
  }

  $form['invite']['node'] = array(
    '#type' => 'value',
    '#value' => $node,
  );

  $form['invite']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Add Invites'),
    '#weight' => 20,
  );

  return $form;
}

function node_invite_add_form_validate($form, &$form_state) {
  // validate sender
  if (!valid_email_address($form_state['values']['your_email'])) {
    form_set_error('your_email',  t('%email is not a valid email address', array('%email' => $form_state['values']['your_email'])));
  }

  // validate file upload
  $validators = array('file_validate_extensions' => array('csv'));
  $file = file_save_upload('csv_upload', $validators);
  if (isset($file) && $file->filemime != 'text/csv') {
  	form_set_error('csv_upload', t('The file must be of CSV type only.'));
  } else {
  	$form_state['values']['csv_upload'] = $file;
  }

  // validate recipients
  $emails = explode("\n", $form_state['values']['emails']);
  $emails = array_filter($emails);
  foreach ($emails as $email) {
    $split = preg_split('/\s*\|\s*/', $email);
    if (count($split) == 2) {
      $email = $split[1];
    }
    $email = trim($email);
    if ($email != '' && !valid_email_address($email)) {
      form_set_error('emails', t('%email is not a valid email address', array('%email' => $email)));
    }
  }

  // Validate all users and store their uids in the 'storage' field to be used
  // by the submit function.
  $users = preg_split('/ *, */', $form_state['values']['users']);
  $users = array_filter($users);
  $form_state['storage']['users'] = array();
  foreach ($users as $user) {
    $query = "SELECT uid FROM {users} WHERE LOWER(name) = LOWER(:name)";
    $uid = db_query($query, array(':name' => trim($user)));
    if (!$uid) {
      form_set_error('users', t('%user is not a valid username', array('%user' => $user)));
    }
    $form_state['storage']['users'][] = $uid;
  }

  // Has anyone been added to the invite?
  if (empty($file) && (!count($emails) && !count($users))) {
    form_set_error('emails', t("Please upload a csv file with people to invite or enter No one has been added to the invite."));
  }
}

function node_invite_add_form_submit($form, &$form_state) {
  global $user;

  $node = $form_state['values']['node'];

  // Create the email address based invites
  $emails_from_upload = node_invite_parse_csv($form_state['values']['csv_upload']);
  $emails_from_form = explode("\n", $form_state['values']['emails']);

  $form_emails = array();
  foreach ($emails_from_form as $recip) {
  	$recipient = explode('|', $recip);
  	if (isset($recipient[1])) {
  		$form_emails[] = $recipient[0] . '| | | |' . $recipient[1];
  	} else {
  		$form_emails[] = $recipient[0] . '| | | |' . $recipient[0];
  	}
  }

  $emails = array_merge($emails_from_upload, $form_emails);
  $emails = array_unique($emails);

  // Remove any empty items
  $emails = array_filter($emails);

  $new_count = 0;
  $updated_count = 0;
  $invites = array();
  if (is_array($emails)) {
    foreach ($emails as $email) {
      $split = preg_split('/\s*\|\s*/', $email);
      $recip_name = $email; // Set name = email unless a name is set
      $recip_mail = $email;
      if (count($split) > 1) {
        $email = trim($split[0]) . ' <'. trim($split[4]) . '>';
        $recip_name = $split[0];
        $recip_company = $split[1];
        $recip_title = $split[2];
        $recip_phone_number = $split[3];
        $recip_mail = $split[4];
      }
      $email = trim($email);

      // Save the invite
      $account = user_load_multiple(array(), array('mail' => $recip_mail));
      $params = array(
        'nid' => $node->nid,
        'email_invitee' => $recip_mail,
        'name_invitee' => $recip_name,
        'company_invitee' => $recip_company,
        'title_invitee' => $recip_title,
        'phone_number_invitee' => $recip_phone_number,
        'uid_invitee' => $account ? $account->uid : 0,
        'uid_inviter' => $user->uid,
        'status' => NODE_INVITE_PENDING,
        'sent' => null,
      	'inviter_custom_name' => $form_state['values']['your_name'],
        'inviter_custom_email' => $form_state['values']['your_email'],
        'personal_message' => $form_state['values']['message'],
      );
      $invite= node_invite_add($recip_mail, $params);

      // Check to see if the invite is new or existing
      if ($invite->new) {
      	$new_count++;
      } else {
      	$updated_count++;
      }
    }
  }

  // Create the user account based invites
  // TODO: If a user was invited in both the email and user fields, then they
  // will receive two invites. We should probably filter them out of the
  // email invites.
  if (is_array($form_state['storage']['users'])) {
    foreach ($form_state['storage']['users'] as $uid) {
      $account = user_load($uid);
      $params = array(
        'nid' => $node->nid,
        'email_invitee' => $account->mail,
        'name_invitee' => $account->name,
        'uid_invitee' => $account->uid,
        'uid_inviter' => $user->uid,
        'status' => NODE_INVITE_SENT,
        'sent' => null,
        'inviter_custom_name' => $form_state['values']['your_name'],
        'inviter_custom_email' => $form_state['values']['your_email'],
        'personal_message' => $form_state['values']['message'],
      );
      $invites[] = node_invite_add($account->mail, $params);
    }
  }

  // Send out the emails
  $send_count = 0;
  foreach ($invites as $invite) {
    // Set the tokens to be replaced in the email.
    node_invite_specific_info($invite, $node);
    if (variable_get('node_invite_set_email', TRUE) || $user->uid == 0) {
      $node->invite_specific_info['inviter-name'] = $form_state['values']['your_name'];
      $node->invite_specific_info['inviter-mail'] = $form_state['values']['your_email'];
    }

    // Send the invite
    node_invite_send_mail($invite, $node);
    $send_count++;
  }

  if ($new_count > 0) {
  	drupal_set_message($new_count . ' invitations were added.');
  }
  if ($updated_count > 0) {
  	drupal_set_message($new_count . ' invitations were updated.');
  }

  unset($form_state['storage']);

  // send them somewhere... back to the node!
  $form_state['redirect'] = "node/" . $node->nid;
}

/*
 * Send out selected invites
 */
function node_invite_send_invite($iids = array()) {
  // Send out the emails
  $send_count = 0;
  foreach ($invites as $invite) {
    // Set the tokens to be replaced in the email.
    node_invite_specific_info($invite, $node);
    if (variable_get('node_invite_set_email', TRUE) || $user->uid == 0) {
      $node->invite_specific_info['inviter-name'] = $form_state['values']['your_name'];
      $node->invite_specific_info['inviter-mail'] = $form_state['values']['your_email'];
    }

    // Send the invite
    node_invite_send_mail($invite, $node);
    $send_count++;
  }

  if ($new_count > 0) {
  	drupal_set_message($new_count . ' invitations were added.');
  }
}

/** Helper Functions **/
function node_invite_parse_csv($theFile) {
  //setup
  $line_max = variable_get('user_import_line_max', 1000);
  ini_set('auto_detect_line_endings', true);
  $filepath = $theFile->destination;
  $handle = @fopen($filepath, "r");
  $row_counter = 0;

  $mails = array();
  while ($row = fgetcsv($handle, $line_max, ',')) {
  	// Skip the header row ($row[0] == 'name')
  	if (strtolower($row[0] == 'name')) {
  		continue;
  	}

  	$mails[] = $row[0] . '|' .$row[1] . '|' .$row[2] . '|' .$row[3] . '|' .$row[4];
  	$row_counter++;
  }
  return $mails;
}
