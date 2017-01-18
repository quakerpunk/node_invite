<?php

class node_invite_invitee_list_handler_field extends views_handler_field_numeric {

  function option_definition() {
    $options = parent::option_definition();
    $options['response_status'] = array('default' => array());
    $options['link_to_invites'] = array('default' => FALSE);
    return $options;
  }

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    $form['response_status'] = array(
      '#type' => 'checkboxes',
      '#title' => t('Select the invite response status to count'),
      '#description' => t('Leaving all options unselected will return the count of all invites'),
      '#default_value' => $this->options['response_status'],
      '#options' => array(
        NODE_INVITE_PENDING => t('Pending'),
        NODE_INVITE_SENT => t('New'),
        NODE_INVITE_RESPONSE_YES => t('Yes'),
        NODE_INVITE_RESPONSE_NO => t('No'),
        NODE_INVITE_RESPONSE_MAYBE => t('Maybe'),
        NODE_INVITE_REVOKED => t('Revoked'),
      ),
    );
    
    $form['link_to_invites'] = array(
      '#title' => t('Link this field to its node invites listing page'),
      '#type' => 'checkbox',
      '#default_value' => $this->options['link_to_invites'],
    );
  }

  // override the query
  function query() {
    $this->field_alias = $this->field;
  }

  function render_link($data, $values) {
    if ($this->options['link_to_invites'] && $data !== NULL && $data !== '') {
      return l($data, 'node/' . $values->nid . '/manage_invites', array('html' => TRUE));
    }
    else {
      return $data;
    }
  }
  
  function render($values) {
    $nid = $values->nid;
    $invites = node_invite_load_invites_by_nid($nid, array_filter($this->options['response_status']));
    $values->{$this->field_alias} = count($invites);
    return $this->render_link(parent::render($values), $values);
  }
}