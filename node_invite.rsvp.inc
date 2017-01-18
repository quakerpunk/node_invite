<?php
/**
 * @file
 * Provides the invite rsvp response form.
 */

function node_invite_rsvp($form, &$form_state, $node, $invite, $inline = FALSE) {
  // this form is probably being accessed by anonymous users.
  $form = array();

  if ($node->nid != $invite->nid) {
    drupal_not_found();
    return;
  }

  node_invite_specific_info($invite, $node);

  if ($invite->email_invitee) {
    $email = $invite->email_invitee;
  }

  if ($invite->name_invitee) {
    $name = $invite->name_invitee;
  }

  if ($inline) {
    $form['rsvp'] = array(
      '#type' => 'fieldset',
      '#description' => '',
      '#collapsed' => FALSE,
      '#collapsible' => FALSE,
    );
  }

  $form['rsvp']['intro'] = array(
    '#markup' => token_replace(
      variable_get('node_invite_rsvp_page_intro', NODE_INVITE_RSVP_PAGE_INTRO),
      array('node' => $node)
    ),
    '#weight' => 0,
  );

  if ($name == $email) {
	  $form['rsvp']['name'] = array(
	    '#type' => 'textfield',
	    '#title' => t('Name'),
	    '#default_value' => $name,
	    '#required' => true,
	    '#weight' => 1,
	  );
  } else {
  	$form['rsvp']['name'] = array(
      '#markup' => '<label>Name</label> <p>' . $name . '</p>',
      '#required' => true,
      '#weight' => 1,
    );
  }

  $form['rsvp']['email'] = array(
      '#markup' => '<label>Email</label> <div>' . $email . '</div>',
      '#required' => true,
      '#weight' => 2,
    );

  $form['rsvp']['company'] = array(
	  '#type' => 'textfield',
	  '#title' => t('Company'),
	  '#default_value' => $invite->company_invitee,
	  '#required' => false,
	  '#weight' => 3,
	);

	$form['rsvp']['title'] = array(
	  '#type' => 'textfield',
	  '#title' => t('Title'),
	  '#default_value' => $invite->title_invitee,
	  '#required' => false,
	  '#weight' => 4,
	);

	$form['rsvp']['phone_number'] = array(
	  '#type' => 'textfield',
	  '#title' => t('Phone Number'),
	  '#default_value' => $invite->phone_number_invitee,
	  '#required' => false,
	  '#weight' => 5,
	);

  if (variable_get('node_invite_rsvp_email', 0)) {
    $form['rsvp']['email'] = array(
      '#type' => 'textfield',
      '#title' => t('Email'),
      '#description' => t('Please enter the email address to which the original invite was sent. This allows us to make sure you are the actual recipient of the invite.'),
      '#required' => TRUE,
      '#weight' => 6,
    );
  }

  $form['rsvp']['attend'] = array(
    '#weight' => 7,
  );
  $form['rsvp']['attend']['pretext'] = array(
    '#markup' => token_replace(
      variable_get('node_invite_rsvp_page_submit_pretext', NODE_INVITE_RSVP_PAGE_SUBMIT_PRETEXT),
      array('node' => $node)
    ),
    '#prefix' => '<div class="node-invite-rsvp-pretext">',
    '#suffix' => '</div>',
  );

  // @TODO: The response words should be configurable settings.
  // i.e. "attending" instead of "yes"
  $form['rsvp']['attend']['status'] = array(
    '#type' => 'radios',
    '#options' => array(
      NODE_INVITE_RESPONSE_YES => t(NODE_INVITE_RESPONSE_YES),
      NODE_INVITE_RESPONSE_NO => t(NODE_INVITE_RESPONSE_NO),
      NODE_INVITE_RESPONSE_MAYBE => t(NODE_INVITE_RESPONSE_MAYBE),
    ),
    '#default_value' => $invite->status == NODE_INVITE_SENT ? NODE_INVITE_RESPONSE_YES : $invite->status,
    '#attributes' => array('class' => 'radios-inline'),
  );

  if (variable_get('node_invite_rsvp_show_comments', 0)) {
    $form['rsvp']['comments'] = array(
      '#type' => 'textarea',
      '#title' => t('Leave a comment for the organizer'),
      '#default_value' => $invite->notes_invitee,
      '#weight' => 8,
    );
  }

  // hidden vars for tracking
  $form['rsvp']['node'] = array(
    '#type' => 'value',
    '#value' => $node,
  );

  if ($invite->uid_invitee) {
    // We should only run into this error with users who are logged in.
    $user = user_load($invite->uid_invitee);
  }

  $form['rsvp']['invite'] = array(
    '#type' => 'value',
    '#value' => $invite,
  );

  $form['rsvp']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Reply'),
    '#weight' => 20,
  );

  $form['rsvp']['node_view'] = array(
    '#markup' => drupal_render(node_view($node)),
  );

  //$form['#redirect'] = 'node/' . $node->nid;
  return $form;
}

function node_invite_rsvp_validate($form, &$form_state) {
  if (intval(variable_get('node_invite_rsvp_email', '0'))) {
    $email = $form_state['values']['email'];
    if (!valid_email_address($email)) {
      // email is not valid.... regardless of whether or not it matches.
      // send them back.
      form_set_error('email', t("$email is not a valid email. Please try again."));
    }
    else {
      $invite = $form_state['values']['invite'];
      if ($email != $invite->email_invitee) {
        form_set_error('email', t("$email is not the address to which this invite was sent.  Please use the <i>exact</i> email to which this invite was sent."));
      }
    }
  }
}

function node_invite_rsvp_submit($form, &$form_state) {
  $invite = $form_state['values']['invite'];

  // Need to double check the status to be translation safe.
  $status = $form_state['values']['status'];
  if (!empty($form_state['values']['comments'])) {
    node_invite_save($invite, array('status' => $status, 'acted_upon' => time(), 'notes_invitee' => $form_state['values']['comments'], 'company_invitee' => $form_state['values']['company'], 'title_invitee' => $form_state['values']['title'], 'phone_number_invitee' => $form_state['values']['phone_number']));
  } else {
  	node_invite_save($invite, array('status' => $status, 'acted_upon' => time(), 'company_invitee' => $form_state['values']['company'], 'title_invitee' => $form_state['values']['title'], 'phone_number_invitee' => $form_state['values']['phone_number']));
  }

  $node = $form_state['values']['node'];
  $admin_notify_setting = variable_get('node_invite_nots_admin', 'site_admin_all');

  // notify the inviter
  $notify_settings = variable_get('node_invite_nots_rsvps', array());
  if (in_array('inviter', $notify_settings) && !empty($notify_settings['inviter'])) {
    $the_message = token_replace(
      variable_get('node_invite_notify_inviter_notify', NODE_INVITE_NOTIFY_INVITER_NOTIFY),
      array('node' => $node)
    );
    $from = variable_get('site_mail', '');

    if ($invite->uid_inviter == $invite->uid_invitee) {
      // This is a self created invite, so notify the node author.
      $to = db_query("SELECT u.mail FROM {users} u JOIN {node} n ON u.uid = n.uid WHERE nid = %d", $invite->nid)->fetch();
    }
    else {
      $to = db_query("SELECT u.mail FROM {users} u JOIN {node_invites} ni ON u.uid = ni.uid_inviter WHERE iid = %d", $invite->iid)->fetch();
    }

    $subject = token_replace(
      variable_get('node_invite_notify_inviter_notify_subject', NODE_INVITE_NOTIFY_INVITER_NOTIFY_SUBJECT),
      array('node' => $node)
    );
    $msg = drupal_mail(
      // module
      'node_invite',
      // key
      'rsvp_inviter',
      // to
      $to,
      // language
      language_default(),
      // params
      array(
        'from'    => $from,
        'subject' => $subject,
        'body'    => $the_message,
      ),
      // from
      $from,
      // send
      TRUE
    );
  }

  // To allow for redirection
  unset($form_state['storage']);
  $invite_status = strtolower($form_state['values']['status']);
  switch($invite_status) {
    case 'yes':
      drupal_set_message(t('Thank you for RSVP\'ing. We look forward to seeing you.'));
      break;
    case 'no':
      drupal_set_message(t('Thank you for RSVP\'ing. We\'ll see you at the next event.'));
      break;
    default:
      drupal_set_message(t('Thank you for RSVP\'ing.'));
      break;
  }

  $rsvp_url = 'node_invite/rsvp/' . $form_state['values']['invite']->nid . '/' . $form_state['values']['invite']->hash;
  //$form_state['redirect'] = 'node/' . $node->nid;
  $form_state['redirect'] = $rsvp_url;
}

function node_invite_rsvp_new($form, &$form_state, $node, $inline = FALSE) {
  global $user;
  $form = array();

  $invite = new stdClass();
  $invite->iid = 0;
  $invite->nid = $node->nid;
  $invite->hash = md5($user->mail . time());
  $invite->uid_invitee = $user->uid;
  $invite->email_invitee = $user->mail;
  $invite->name_invitee = '';
  $invite->company_invitee = '';
  $invite->title_invitee = '';
  $invite->phone_number_invitee = '';
  $invite->status = NODE_INVITE_SENT;
  $invite->notes_invitee = '';
  $invite->uid_inviter = 1;
  $invite->personal_message = null;

  $default_rsvp_form = node_invite_rsvp($form, $form_state, $node, $invite);
  unset($default_rsvp_form['rsvp']['attend']['pretext']);
  unset($default_rsvp_form['rsvp']['attend']['status']);

  if ($inline) {
    $form['rsvp'] = array(
      '#type' => 'fieldset',
      '#title' => t('RSVP to this @type', array('@type' => $node->type)),
      '#description' => '',
      '#collapsed' => TRUE,
      '#collapsible' => TRUE,
    );
  }

  if ($user->uid) {
  	$form['rsvp']['name'] = array(
      '#type' => 'value',
      '#markup' => $user->name,
    );
    $form['rsvp']['email'] = array(
      '#type' => 'value',
      '#markup' => $user->mail,
    );
  }
  else {
  	$form['rsvp']['name'] = array(
      '#type' => 'textfield',
      '#title' => t('Name'),
      '#default_value' => '',
  	  '#required' => true,
    );
    $form['rsvp']['email'] = array(
      '#type' => 'textfield',
      '#title' => t('Email'),
      '#default_value' => '',
      '#required' => true,
    );
  }

  $form['rsvp']['company'] = array(
	  '#type' => 'textfield',
	  '#title' => t('Company'),
	  '#default_value' => $invite->company_invitee,
	  '#required' => true,
	);

	$form['rsvp']['title'] = array(
	  '#type' => 'textfield',
	  '#title' => t('Title'),
	  '#default_value' => $invite->title_invitee,
	  '#required' => false,
	);

	$form['rsvp']['phone_number'] = array(
	  '#type' => 'textfield',
	  '#title' => t('Phone Number'),
	  '#default_value' => $invite->phone_number_invitee,
	  '#required' => false,
	);

  $form['rsvp']['attend'] = $default_rsvp_form['rsvp']['attend'];
  $form['rsvp']['comments'] = $default_rsvp_form['rsvp']['comments'];
  $form['rsvp']['submit'] = $default_rsvp_form['rsvp']['submit'];
  $form['rsvp']['submit']['#value'] = t('Register');
  $form['rsvp']['node'] = $default_rsvp_form['rsvp']['node'];

  $form['#validate'][] = 'node_invite_rsvp_validate';
  $form['#validate'][] = 'node_invite_rsvp_new_validate';
  //$form['#redirect'] = 'node/' . $node->nid;

  return $form;
}

function node_invite_rsvp_new_validate($form, &$form_state) {
  // Does the email address match an existing anonymous invitation?
  $node = $form_state['values']['node'];
  if ($iid = db_query("SELECT iid FROM {node_invites} WHERE email_invitee = :email AND nid = :nid", array(':email' => $form_state['values']['email'], ':nid' => $node->nid))->fetchField()) {
    $invite = node_invite_load($iid);
    if ($invite->uid_invitee) {
      form_set_error('email', t('An invitation for a user with that email already exists. Please login to the site to edit your RSVP'));
    }
    $form_state['values']['invite'] = $invite;
  }

}

function node_invite_rsvp_new_submit($form, &$form_state) {
  global $user;
  $node = $form_state['values']['node'];
  $params = array(
    'nid' => $node->nid,
    'email_invitee' => $form_state['values']['email'],
    'name_invitee' => $form_state['values']['name'],
    'company_invitee' => $form_state['values']['company'],
    'title_invitee' => $form_state['values']['title'],
    'phone_number_invitee' => $form_state['values']['phone_number'],
    'uid_invitee' => $user->uid,
    'uid_inviter' => $user->uid,
    'status' => (isset($form_state['values']['status'])) ? $form_state['values']['status'] : NODE_INVITE_RESPONSE_YES,
    'notes_invitee' => $form_state['values']['comments'],
  );

  if (isset($form_state['values']['invite'])) {
    $invite = node_invite_save($form_state['values']['invite'], $params);
  }
  else {
    $invite = node_invite_save(NULL, $params);
  }

  // Set a cookie for the user if they're anon so that we can track their
  // response for this session.
  if (!$user->uid) {
    $_SESSION['node_invites'][$node->nid] = $invite->iid;
  }

  drupal_set_message(t('Your registration has been submitted.  We look forward to seeing you at the event.'));
  //$form_state['redirect'] = 'node/' . $node->nid;
}

function node_invite_find_invite($node, $redir = TRUE) {
  global $user;
  $invite = FALSE;

  if (($iid = db_query("SELECT iid FROM {node_invites} WHERE uid_invitee = :invitee_id AND nid = :nid", array(':invitee_id' => $user->uid, ':nid' => $node->nid))->fetchField()) && $user->uid) {
    $invite = node_invite_load($iid);
  }
  elseif ($_SESSION['node_invites'][$node->nid]) {
    $invite = node_invite_load($_SESSION['node_invites'][$node->nid]);
  }

  if (!$redir) {
    return $invite;
  }

  if ($invite) {
    drupal_goto(url(node_invite_rsvp_path($invite)));
  }
  elseif ($node->node_invite_settings['open']) {
    drupal_goto(url("node_invite/rsvp_new/$node->nid"));
  }

  drupal_access_denied();
}

/**
 * Used by anonymous users to clear their RSVP session data if they need
 * to RSVP under a different email address
 */
function node_invite_clear_rsvp_session($node) {
  unset($_SESSION['node_invites']);
  drupal_goto('node/' . $node->nid);
}

