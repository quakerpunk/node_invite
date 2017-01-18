<?php

define('NODE_INVITE_RSVP_PAGE_SUBMIT_PRETEXT', 'Will you accept this invite?');
define('NODE_INVITE_RSVP_PAGE_TITLE', 'RSVP For: [node:title]');
define('NODE_INVITE_MESSAGE_DEFAULT', '<p>[node_invite:inviter-name] invites you to rsvp for a [node:content-type]!  </p><p> Visit <a href="[node_invite:inviter-node-url]">[node:title]</a> for more information or just check out the <a href="[node_invite:inviter-rsvp-url]">RSVP Page</a> to confirm your attendance!</p><br />[node_invite:node-invite-personal-message]');
define('NODE_INVITE_SUBJECT_DEFAULT', "[node_invite:inviter-name] at [site:name] has invited you to a [node:content-type] called [node:title]!");
define('NODE_INVITE_NOTIFY_SITE_ADMIN_YES_SUBJECT', '[site:name] - Invite ACCEPT notification');
define('NODE_INVITE_RSVP_PAGE_INTRO', "<h2>[node_invite:inviter-name] at [site:name] has invited you to a [node:content-type] called [node:title]!</h2>\n<p>Use this form to confirm (or deny) attendance.</p>");
define('NODE_INVITE_NOTIFY_SITE_ADMIN_YES', 'An invite was accepted for node [nid]. Login to <a href="[site-url]">the site</a> to view it.');
define('NODE_INVITE_NOTIFY_SITE_ADMIN_ALL_SUBJECT', '[site-name] - Invite response notification');
define('NODE_INVITE_NOTIFY_SITE_ADMIN_ALL', 'An invite was acted upon for node [node:nid]. Login to <a href="[site:url]">the site</a> to view it.');
define('NODE_INVITE_NOTIFY_INVITEE_CONFIRM_SUBJECT', '[site:name] - Invite response confirmation');
define('NODE_INVITE_NOTIFY_INVITEE_CONFIRM', 'Your invite selection for [title] has been processed. Thanks!');
define('NODE_INVITE_NOTIFY_INVITER_NOTIFY_SUBJECT', '[site:name] - Invite response!');
define('NODE_INVITE_NOTIFY_INVITER_NOTIFY', 'Your invite selection for [node:title] has been acted upon. Please login to <a href="[site:url]/?q=user">the site</a> and click "Invites" to view it. Thanks!');

/* added to enable per-node override with a checkbox */
define('NODE_INVITE_DEFAULT_FOR_ENABLED_NODES', 1);

/* Constants */
define('NODE_INVITE_RESPONSE_YES', 'Yes');
define('NODE_INVITE_RESPONSE_NO', 'No');
define('NODE_INVITE_RESPONSE_MAYBE', 'Maybe');
define('NODE_INVITE_REVOKED', 'Revoked');
define('NODE_INVITE_SENT', 'Sent');
define('NODE_INVITE_PENDING', 'Pending');

