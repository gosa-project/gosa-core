function question(text, url)
{
  if(document.mainform.ignore){
	location.href= url;
	return true;
  }
  if(confirm(text)){
	location.href= url;
  	return true;
  }
  return false;
}

