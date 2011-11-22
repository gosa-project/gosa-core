<?php

class csvExporter
{
  var $result;

  function csvExporter($headline, $header, $entries, $columns= array()) {
    // If no preset, render all columns
    if (!count($columns)) {
      foreach ($header as $index => $dummy) {
        $columns[]= $index;
      }
    }
    
    // Generate header
    $this->result= "#";
    foreach ($columns as $index) {
      if (isset($header[$index])){
        $this->result.= trim($header[$index]).";";
      } else {
        $this->result.= ";";
      }
    }
    $this->result= preg_replace('/;$/', '', $this->result)."\n";

    // Append entries
    foreach ($entries as $row) {
      foreach ($columns as $index) {
        if (isset($row["_sort$index"])){
          $this->result.= trim($row["_sort$index"]).";";
        } else {
          $this->result.= ";";
        }
      }
      $this->result= preg_replace('/;$/', '', $this->result)."\n";
    }
  }


  function query()
  {
     return $this->result;
  }


  static function getInfo()
  {
    return array("exportCVS" => array( "label" => _("CSV"), "image" => "images/lists/csv.png", "class"=> "csvExporter", "mime" => "text/x-csv", "filename" => "export.csv" ));
  }

}

?>
