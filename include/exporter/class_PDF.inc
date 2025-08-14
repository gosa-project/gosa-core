<?php

/* Avoid autoloading this class */ class PDF extends FPDF
{
  var $headline= "";

  function setHeadline($headline)
  {
    $this->headline= $headline;
  }


  function Header()
  {
      $this->SetFont('Helvetica','B',10);
      $this->Cell(0, 0, $this->headline, 0, 0, 'L');
      $this->Ln(5);
  }
  
  function Footer()
  {
      $this->SetY(-15);
      $this->SetFont('Helvetica','I',8);
      $this->Cell(0,10, _("Page")." ".$this->PageNo().'/{nb}',0,0,'C');
  }

}

?>
