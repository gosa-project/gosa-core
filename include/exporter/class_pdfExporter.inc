<?php

// Try to load PDF library
define('FPDF_FONTPATH', '/usr/share/php/fpdf/font/');
include('fpdf/fpdf.php');

class pdfExporter
{
  var $result;

  function pdfExporter($headline, $header, $entries, $columns= array()) {
    // If no preset, render all columns
    if (!count($columns)) {
      foreach ($header as $index => $dummy) {
        $columns[]= $index;
      }
    }

    // Create new PDF
    $this->result=new FPDF('L', 'mm', 'A4');
    $this->result->SetFont('Helvetica', '', 10);
    $this->result->AddPage();

    // Analyze for width
    $width= $this->calcWidth($header, $entries, $columns);
    
    // Render head
    $this->result->SetFont('','B');
    $this->result->Cell(0,0,utf8_decode($headline),0,0,'L');
    $this->result->Ln(5);

    // Generate header
    $this->result->SetFillColor(240,240,240);
    $this->result->SetTextColor(0);
    $this->result->SetDrawColor(0,0,0);
    $this->result->SetLineWidth(.3);
    $this->result->SetFont('','B');

    foreach ($columns as $order => $index) {
      if (isset($header[$index])){
        $this->result->Cell($width[$order], 7, utf8_decode($header[$index]), 1, 0, 'C', 1);
      } else {
        $this->result->Cell($width[$order], 7, '', 1, 0, 'C', 1);
      }
    }
    $this->result->Ln();

    // Append entries
    $this->result->SetFillColor(224,235,255);
    $this->result->SetTextColor(0);
    $this->result->SetFont('');

    $fill= false;
    foreach ($entries as $row) {
      foreach ($columns as $order => $index) {

        if (isset($row["_sort$index"])){
          $this->result->Cell($width[$order], 6, utf8_decode($row["_sort$index"]), 'LR', 0, 'L', $fill);
        } else {
          $this->result->Cell($width[$order], 6, '', 'LR', 0, 'L', $fill);
        }
      }

      $this->result->Ln();
      $fill= !$fill;
    }
    $this->result->Cell(array_sum($width), 0, '', 'T');
  }


  function calcWidth($header, $entries, $columns)
  {
    $width= array();

    // Locate longest value for each column
    foreach ($columns as $order => $index) {
      $max= 0;

      if (isset($header[$index])){
        $len= $this->result->GetStringWidth($header[$index]);
        if ($len > $max) {
          $max= $len;
        }
      }

      foreach ($entries as $row) {
        if (isset($row["_sort$index"])){
          $len= $this->result->GetStringWidth($row["_sort$index"]);
          if ($len > $max) {
            $max= $len;
          }
        }
      }

      $width[]= $max;
    }

    // Scale to page width
    $printWidth= 280;
    $scale= $printWidth / array_sum($width);
    foreach ($width as $index => $w) {
      $width[$index]= (int)($width[$index] * $scale);
    }

    return $width;
  }


  function query()
  {
     return $this->result->Output("", "S");
  }


  static function getInfo()
  {
    // Check if class defined
    $classes= get_declared_classes();
    if(in_array('FPDF', $classes)) {
      return array("exportPDF" => array( "label" => _("PDF"), "image" => "images/lists/pdf.png", "class"=> "pdfExporter", "mime" => "application/pdf", "filename" => "export.pdf" ));
    } else {
      return null;
    }

  }

}

?>
