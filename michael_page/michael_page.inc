<?php 

function import_employees(){
  $url = variable_get('api_endpoint','http://dummy.restapiexample.com/api/').variable_get('api_version','v1')."/".variable_get('method_route','employees');
  $batch = array(
    'title' => t('Creating employees ...'),
    'operations' => array(),
    'file' => drupal_get_path('module', 'michael_page') . '/michael_page.inc',
    'init_message' => t('Commencing'),
    'progress_message' => t('Processed @current out of @total.'),
    'error_message' => t('An error occurred during processing')
  );
  $employees_list = json_decode(drupal_http_request($url)->data);
  foreach ($employees_list as $employee) {
     $batch['operations'][] = array('_create_employe', array($employee->id,$employee->employee_name,$employee->employee_salary,$employee->employee_age));
  }
  batch_set($batch);
  batch_process('employees');
}

function _create_employe($remote_id, $employee_name, $employee_salary, $employee_age, &$context){
  $result = db_query("SELECT entity_id FROM {field_data_remote_id} WHERE remote_id_value = :remote_id", array(':remote_id' => $remote_id))->fetchField();
  if($result) {
    $employee_node = node_load($result);
    watchdoG("aviso","duplicado!");
  } else {
    global $user;
    $employee_node = new stdclass();
    $employee_node->title = $employee_name;
    $employee_node->name[$employee_node->language][]['value'] = $employee_name;
    $employee_node->type = "employed";
    $employee_node->language = LANGUAGE_NONE;
    $employee_node->uid = $user->uid;
    $employee_node->status = 1;
    $employee_node->promote = 0;
    $employee_node->comment = 0;    
  }
  $employee_node->remote_id[$employee_node->language][]['value'] = $remote_id;
  $meployee_node->employ_name[$employee_node->language][]['value'] = $employee_name;
  $employee_node->age[$employee_node->language][]['value'] = $employee_age;
  if($employee_salary<=1000){
    $salary_term = taxonomy_get_term_by_name("Salario bajo","salary_type");
  }elseif($employee_salary>1000  &&  $employee_salary<=4000){
    $salary_term = taxonomy_get_term_by_name("Salario medio","salary_type");
  }else{
    $salary_term = taxonomy_get_term_by_name("Salario alto","salary_type");
  }
  $employee_node->salary_type[$employee_node->language][]['tid'] = reset($salary_term)->tid;
  $employee_node = node_submit($employee_node);
  node_save($employee_node);
  $context['message'] = t('Processing employee @name', array('@name' => $remote_id));
  $context['results'][] = $remote_id;
}