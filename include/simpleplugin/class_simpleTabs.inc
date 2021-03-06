<?php
/*
  This code is part of FusionDirectory (http://www.fusiondirectory.org/)
  Copyright (C) 2013  FusionDirectory

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
  Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
*/

/*
 * simpleTabs class
 * The idea is to avoid writing your own tab class. You can use this one instead,
 * you'll just have to implement the compute_dn method in your main tab.
 * This method should of course return a string containing the dn for the object.
 * */
class simpleTabs extends tabs
{
  protected $specialTabs = TRUE;

  function __construct($config, $data, $dn, $category, $copied_object = NULL)
  {
    parent::tabs($config, $data, $dn, $category, $copied_object);

    if ($this->specialTabs) {
      /* Add references/acls/snapshots */
      $this->addSpecialTabs();
    }
  }

  function save()
  {
    $baseobject = $this->getBaseObject();
    $new_dn     = $baseobject->compute_dn();
    @DEBUG (DEBUG_TRACE, __LINE__, __FUNCTION__, __FILE__, $new_dn, "Saving");

    /* Move ? */
    if ($this->dn != $new_dn) {
      /* Write entry on new 'dn' */
      if ($this->dn != 'new') {
        $baseobject->move($this->dn, $new_dn);
      }
      /* Happen to use the new one */
      $this->dn = $new_dn;
    }

    return parent::save();
  }
}

class simpleTabs_noSpecial extends simpleTabs
{
  protected $specialTabs = FALSE;
}
?>
