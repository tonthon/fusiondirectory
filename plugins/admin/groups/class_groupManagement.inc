<?php

/*
  This code is part of FusionDirectory (http://www.fusiondirectory.org/)
  Copyright (C) 2003  Cajus Pollmeier
  Copyright (C) 2011-2013  FusionDirectory

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
*/

class groupManagement extends management
{
  // Tab definition
  protected $tabClass = "grouptabs";
  protected $tabType = "GROUPTABS";
  protected $aclCategory = "groups";
  protected $aclPlugin   = "group";
  protected $objectName   = "group";

  /* Return plugin informations for acl handling */
  static function plInfo()
  {
    return array(
      "plShortName"   => _("Groups"),
      "plDescription" => _("Manage groups"),
      "plIcon"        => "plugins/groups/images/icon.png",
      "plSection"     => "admin",
      "plCategory"    => array("groups"),
      "plPriority"    => 2,

      "plProvidedAcls" => array()
    );
  }

  function __construct($config,$ui)
  {
    $this->config = $config;
    $this->ui = $ui;

    $this->storagePoints = array(get_ou("groupRDN"));

    // Build filter
    if (session::global_is_set(get_class($this)."_filter")){
      $filter= session::global_get(get_class($this)."_filter");
    } else {
      $filter = new filter(get_template_path("group-filter.xml", true));
      $filter->setObjectStorage($this->storagePoints);
    }
    $this->setFilter($filter);

    // Build headpage
    $headpage = new listing(get_template_path("group-list.xml", true));
    $headpage->registerElementFilter("filterProperties", "groupManagement::filterProperties");
    $headpage->setFilter($filter);

    parent::__construct($config, $ui, "groups", $headpage);

    $this->registerAction("edit_group","editEntry");
    $this->registerAction("edit_environment","editEntry");
    $this->registerAction("edit_appgroup","editEntry");
    $this->registerAction("edit_mailgroup","editEntry");
  }


  function editEntry($action="",$target=array(),$all=array(), $altTabClass ="", $altTabType = "", $altAclCategory="")
  {
    $str = management::editEntry($action,$target);

    if(preg_match("/^edit_/",$action)){
      $tab = preg_replace("/^edit_/","",$action);
      if(isset($this->tabObject->by_object[$tab])){
        $this->tabObject->current = $tab;
      }else{
        trigger_error("Unknown tab: ".$tab);
      }
    }
    if(!empty($str)) return($str);
  }

  static function filterProperties($row, $classes)
  {
    $result= "";

    $map = array(
        "posixGroup" =>
        array(
          "image" => "plugins/groups/images/iconMini.png",
          "plugin" => "group",
          "alt" => _("Posix"),
          "title" => _("Edit posix properties")
          ),

        "gosaMailAccount" =>
        array(
          "image" => "plugins/groups/images/mail.png",
          "plugin" => "mailgroup",
          "alt" => _("Mail"),
          "title" => _("Edit mail properties")
          ),

        "sambaGroupMapping" =>
        array(
          "image" => "plugins/groups/images/samba.png",
          "plugin" => "group",
          "alt" => _("Samba"),
          "title" => _("Edit samba properties")
          ),

        "goFonPickupGroup" =>
          array(
              "image" => "plugins/groups/images/asterisk.png",
              "plugin" => "group",
              "alt" => _("Phone"),
              "title" => _("Edit phone properties")
              ),

        "gotoMenuGroup" =>
          array(
              "image" => "plugins/groups/images/menu.png",
              "plugin" => "appgroup",
              "alt" => _("Menu"),
              "title" => _("Edit start menu properties")
              ),

        "gotoEnvironment" =>
          array(
              "image" => "plugins/groups/images/environment.png",
              "plugin" => "environment",
              "alt" => _("Environment"),
              "title" => _("Edit environment properties")
              )
          );


    // Walk thru map
    foreach ($map as $oc => $properties) {
      if (in_array_ics($oc, $classes)) {
        $result.="<input class='center' type='image' src='".$properties['image']."' ".
          "alt='".$properties['alt']."' title='".$properties['title'].
                 "' name='listing_edit_".$properties['plugin']."_$row' style='padding:1px'>";
      } else {
        $result.="<img src='images/empty.png' alt=' ' class='center' style='padding:1px'>";
      }
    }
    return $result;
  }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
