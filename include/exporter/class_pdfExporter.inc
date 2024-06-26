<?php
// Try to load PDF library
if (file_exists('fpdf/fpdf.php')) {
  @include_once('fpdf/fpdf.php');
}

// Load supporter class only if FPDF is loaded
$classes= get_declared_classes();
if(in_array_strict('FPDF', $classes)) {
  include('class_PDF.inc');
}

class pdfExporter
{
  var $result;

  function __construct($headline, $header, $entries, $columns= array()) {
    // Bail out if no FPDF available
    if(!class_exists('FPDF')) {
      die(_("No PDF export possible: there is no FPDF library installed."));
    }

    // If no preset, render all columns
    if (!count($columns)) {
      foreach ($header as $index => $dummy) {
        $columns[]= $index;
      }
    }

    // Create new PDF
    $this->result=new PDF('L', 'mm', 'A4');
    $this->result->AliasNbPages();
    $this->result->SetFont('Helvetica', '', 10);
    $this->result->setHeadline(utf8_decode($headline));
    $this->result->AddPage();

    // Analyze for width
    $width= $this->calcWidth($header, $entries, $columns);
    
    // Render head
    $this->result->SetFont('','B');
    $this->result->SetTextColor(0);
    $this->result->SetDrawColor(0,0,0);
    $this->result->SetLineWidth(.3);

    // Height calculator
    $height= 0;

    $fill= false;
    foreach ($entries as $row) {
      // Render header
      if ($height == 0) {
        // Generate header
        $this->result->SetFillColor(230,230,230);
        $this->result->SetFont('','B');

        foreach ($columns as $order => $index) {
          if (isset($header[$index])){
            $this->result->Cell($width[$order], 7, utf8_decode($header[$index]), 1, 0, 'C', 1);
          } else {
            $this->result->Cell($width[$order], 7, '', 1, 0, 'C', 1);
          }
        }
        $this->result->Ln();
        $height= 7;

        // Set entry collors
        $this->result->SetFillColor(240,240,240);
        $this->result->SetFont('');
      }

      foreach ($columns as $order => $index) {

        if (isset($row["_sort$index"])){
          $this->result->Cell($width[$order], 6, utf8_decode($row["_sort$index"]), 'LR', 0, 'L', $fill);
        } else {
          $this->result->Cell($width[$order], 6, '', 'LR', 0, 'L', $fill);
        }
      }

      $this->result->Ln();

      // Increase height to eventually create new page
      $height+= 8;
      if ($height > 220) {
        $height= 0;
        $this->result->Cell(array_sum($width), 0, '', 'T');
        $this->result->AddPage();
        $fill= false;
      } else {
        $fill= !$fill;
      }
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
    if(in_array_strict('FPDF', $classes)) {
      return array("exportPDF" => array( "label" => _("PDF"), "image" => "images/lists/pdf.png", "class"=> "pdfExporter", "mime" => "application/pdf", "filename" => "export.pdf" ));
    } else {
      return null;
    }

  }

}

?>
