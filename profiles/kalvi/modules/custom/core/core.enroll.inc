<?php
/*
 * Returns a form with all students who are not enrolled to this course
 */
function course_enroll_form($form, &$form_state, $nid) {
    if (!empty($nid)) {
        $form['students'] = array(
            '#type' => 'checkboxes',
            '#title' => t('Students'),
            '#attributes' => array('class' => array('students')),
            // '#default_value' => !empty($syl['value']) ? $syl['value'] : '',
            '#options' => get_unenrolled_students($nid),
        );
        $form['course_id'] = array(
            '#type' => 'hidden',
            '#default_value' => $nid,
        );
        $form['submit'] = array(
            '#type' => 'submit',
            '#value' => t('Enroll'),
        );

        $form['#submit'][] = 'course_enroll_form_submit';
    }

    return $form;
}

function course_enroll_form_submit($form, &$form_state) {
    $users_to_enroll = $form_state['values']['students'];
    $nid = $form_state['values']['course_id'];
    _enroll_users_to_course($users_to_enroll, $nid);
}

/*
 * Takes course id as input
 * Returns all students who are not enrolled to given course
 */
function get_unenrolled_students($course_id, $role='student') {
    $students = array();
    $endpoint_users = array();
    //get users enrolled in this course
    $endpoints = _get_enrolled_students($course_id);
    $endpoint_users = array();
    if (!empty($endpoints['users'])) {
        $endpoint_users = $endpoints['users'];
    }
    $role = user_role_load_by_name($role);
    $result = db_select('users_roles', 'ur')
            ->fields('ur', array('uid'))
            ->condition('rid', $role->rid);
    if (!empty($endpoint_users)) {
        $result = $result->condition('ur.uid', $endpoint_users, 'NOT IN');
    }
    $result->join('users', 'u', 'u.uid = ur.uid');

    $result = $result->fields('u', array('name'))->execute();

    foreach ($result as $record) {
        $students[$record->uid] = $record->name;
    }

    return $students;
}

/*
 * Takes course id as input
 * Returns all students enrolled to given course
 */
function _get_enrolled_students($course_id) {
  $rel = course_progress_relation_get('node', $course_id, 'enrollment');
  if (empty($rel['enrollment'])) {
    return array();
  }
  $endpoint_users['rid'] = $rel['enrollment'][0]->rid;
  //update the relation, from second time
  $relation = $rel['enrollment'][0];
  $endpoints = $relation->endpoints[LANGUAGE_NONE];
  foreach ($endpoints as $endpoint) {
    if ($endpoint['entity_type'] == 'user') {
      $endpoint_users['users'][] = $endpoint['entity_id'];
    }
  }
  return $endpoint_users;
}

/*
 * Extension of relation_get_related_entity
 * Returns all entities that are related to the given entity
 */
function relation_get_related_entities($entity_type, $entity_id, $relation_type = NULL, $r_index = NULL) {
    $query = relation_query($entity_type, $entity_id, $r_index);
    if ($relation_type) {
        $query->entityCondition('bundle', $relation_type);
    }
    $results = $query->execute();
    $result = reset($results);
    if (empty($result)) {
        return FALSE;
    }
    // Loop through each relation
    foreach ($results as $relation) {
        $rids[] = $relation->rid;
    }
    // Load all related entities endpoint references
    $raw_relations = relation_load_multiple($rids);

    // Reformat relations output.
    foreach ($raw_relations as $relation) {
        if (!isset($relations[$relation->relation_type])) {
            $relations[$relation->relation_type] = array();
        }
        $relations[$relation->relation_type][] = $relation;
    }

    return $relations;
}

/*
 * Takes a list of users and course id
 * Enrolls all users in list to the given course id
 */
function _enroll_users_to_course($users, $course_id) {
    $endpoints = _get_enrolled_students($course_id);
    $endpoint_users = array();
    if (!empty($endpoints['users'])) {
        $endpoint_users = $endpoints['users'];
    }
    $endpoints_new[] = array('entity_type' => 'node', 'entity_id' => $course_id);
    foreach ($endpoint_users as $endpoint) {
        $endpoints_new[] = array('entity_type' => 'user', 'entity_id' => $endpoint);
    }
    foreach ($users as $user) {
        if ($user != 0)
            $endpoints_new[] = array('entity_type' => 'user', 'entity_id' => $user);
    }
    if (!empty($endpoints['rid'])) {
        $rid = $endpoints['rid'];
        $relation = relation_load($rid);
        $enroll = relation_create('enrollment', $endpoints_new);
        $relation->endpoints = $enroll->endpoints;
        //update current relation
        relation_update($relation);
        return;
    }
    //create new relation
    $enroll = relation_create('enrollment', $endpoints_new);
    return relation_save($enroll);
}
