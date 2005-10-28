<?php
// La séquence de base avec LDAP est 
// connexion, liaison, recherche, interprétation du résultat
// déconnexion
 $admin="cn=admin GOsa main administrator,dc=vincent,dc=be";
 $password="";
echo '<h3>requête de test de LDAP</h3>';
echo 'Connexion ...';
$ds=ldap_connect("localhost");  // doit être un serveur LDAP valide !
echo 'Le résultat de connexion est ' . $ds . '<br />';

if ($ds) { 
    echo 'Liaison ...'; 
    $r=ldap_bind($ds);     // connexion anonyme, typique
//      $r=ldap_bind($ds);                                 // pour un accès en lecture seule.
     echo 'Le résultat de connexion est ' . $r . '<br />';
// 
     echo 'Recherchons (sn=S*) ...';
//     // Recherche par nom
//      $sr=ldap_search($ds,"o=My Company, c=US", "sn=S*");
     $sr=   @ldap_search ($ds, "", "objectClass=*", array("namingContexts"));
     echo 'Le résultat de la recherche est ' . $sr . '<br />';
// 
//     echo 'Le nombre d\'entrées retournées est ' . ldap_count_entries($ds,$sr) 
//          . '<br />';
// 
//     echo 'Lecture des entrées ...<br />';
//     $info = ldap_get_entries($ds, $sr);
//     echo 'Données pour ' . $info["count"] . ' entrées:<br />';
// 
//     for ($i=0; $i<$info["count"]; $i++) {
//         echo 'dn est : ' . $info[$i]["dn"] . '<br />';
//         echo 'premiere entree cn : ' . $info[$i]["cn"][0] . '<br />';
//         echo 'premier email : ' . $info[$i]["mail"][0] . '<br />';
    
    
    echo 'Fermeture de la connexion';
    ldap_close($ds);

} else {
    echo '<h4>Impossible de se connecter au serveur LDAP.</h4>';
}
?> 