<?php
/*
  This code is part of FusionDirectory (http://www.fusiondirectory.org/)
  Copyright (C) 2012-2013 FusionDirectory

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

/* Debug level is an OR combination of several values */
class DebugLevelAttribute extends SetAttribute
{
  protected function loadAttrValue ($attrs)
  {
    if (isset($attrs[$this->getLdapName()])) {
      $value = $attrs[$this->getLdapName()][0];
      $this->value = array();
      foreach ($this->attribute->getChoices() as $choice) {
        if ($value & $choice) {
          $this->value[] = $choice;
        }
      }
    } else {
      $this->resetToDefault();
    }
  }

  function computeLdapValue()
  {
    $value = 0;
    foreach ($this->value as $v) {
      $value |= $v;
    }
    return $value;
  }
}

class configInLdap extends simplePlugin
{
  var $objectclasses  = array("fusionDirectoryConf");
  var $mainTab        = TRUE;

  static function plInfo()
  {
    return array(
      "plShortName"     => _("Configuration"),
      "plDescription"   => _("FusionDirectory configuration"),
      "plIcon"          => "plugins/config/images/icon.png",
      "plSelfModify"    => FALSE,
      "plCategory"      => array("configuration"),
      "plObjectType"    => array("configuration" => array("name"  => _("FusionDirectory configuration"),
                                                          "filter"  => "objectClass=fusionDirectoryConf")),
      "plSection"       => array("addons" => array("name" => _("Addons"), "priority" => 10)),
      "plPriority"      => 0,

      "plProvidedAcls"  => parent::generatePlProvidedAcls(self::getAttributesInfo())
    );
  }

  static function getAttributesInfo ()
  {
    $plugins = array();
    if (session::global_is_set('plist')) {
      $plugins = array_keys(session::global_get('plist')->info);
    }
    return array(
      'look_n_feel' => array(
        'name'  => _('Look n feel'),
        'attrs' => array(
          new SelectAttribute (
            _('Language'), _('Language of the application. If \'automatic\' or not available, the one asked by the browser will be used. This setting can be overriden per user.'),
            'fdLanguage', FALSE,
            array('')
          ),
          new SelectAttribute (
            _('Theme'), _('Theme to be used'),
            'fdTheme', TRUE,
            self::get_themes(),
            'default'
          ),
          new SelectAttribute (
            _('Timezone'), _('Timezone to be used'),
            'fdTimezone', TRUE,
            array('America/New_York')
          ),
        )
      ),
      'schema_setup' => array(
        'name'  => _('Schema setup'),
        'attrs' => array(
          new BooleanAttribute (
            _('rfc2307bis'),
            _('Enables rfc2307bis style groups. You can use member attributes instead of memberUid in this case.'),
            'fdRfc2307bis', FALSE,
            FALSE
          ),
          new BooleanAttribute (
            _('schemaCheck'),
            _('Enables schema checking during login.'),
            'fdSchemaCheck', FALSE,
            FALSE
          ),
        )
      ),
      'password' => array(
        'name'  => _('Password settings'),
        'attrs' => array(
          new SelectAttribute (
            _('Password default hash'), _('Default hash to be used'),
            'fdPasswordDefaultHash', TRUE,
            array('crypt/md5')
          ),
          new BooleanAttribute (
            _('Force default hash'), _('Force the use of the default password hash'),
            'fdForcePasswordDefaultHash'
          ),
          new IntAttribute (
            _('Password minimum length'), _('Minimum length of user passwords'),
            'fdPasswordMinLength', FALSE,
            0 /*min*/, FALSE /*no max*/
          ),
          new IntAttribute (
            _('Password minimum differs'), _('Minimum number of different characters from last password'),
            'fdPasswordMinDiffer', FALSE,
            0 /*min*/, FALSE /*no max*/
          ),
          new StringAttribute (
            _('Password hook'), _('External script to handle password settings'),
            'fdPasswordHook'
          ),
          new BooleanAttribute (
            _('Use account expiration'),
            _('Enables shadow attribute tests during the login to FusionDirectory and forces password renewal or account locking'),
            'fdHandleExpiredAccounts'
          ),
          new StringAttribute (
            _('SASL Realm'), _('SASL Realm'),
            'fdSaslRealm'
          ),
          new StringAttribute (
            _('SASL Exop'), _('Attribute to be stored in the userPassword attribute'),
            'fdSaslExop'
          ),
        )
      ),
      'core_settings' => array(
        'name'  => _('Core settings'),
        'attrs' => array(
          new BooleanAttribute (
            _('Enable primary group filter'),
            _('It is time consuming to evaluate which groups are primary and which are not, so you may want to disable it if your group plugin is slow.'),
            'fdPrimaryGroupFilter', FALSE,
            TRUE
          ),
          new BooleanAttribute (
            _('Display summary in listings'),
            _('Determines whether a status bar will be shown on the bottom of lists, displaying a short summary of type and number of elements in the list.'),
            'fdListSummary', FALSE,
            TRUE
          ),
          new BooleanAttribute (
            _('Honour administrative units'),
            _('Enables  checking  of unitTag attributes when using administrative units.'),
            'fdHonourUnitTags'
          ),
          new SelectAttribute (
            _('Edit locking'),
            _('Check if a entry currently being edited has been modified outside of FusionDirectory in the meantime.'),
            'fdModificationDetectionAttribute', FALSE,
            array('', 'entryCSN', 'contextCSN')
          ),
          new BooleanAttribute (
            _('Enable Copy & Paste'),
            _('Enables copy and paste for LDAP entries managed with FusionDirectory.'),
            'fdCopyPaste'
          ),
          new BooleanAttribute (
            _('Enable logging'),
            _('Event logging on FusionDirectory side.'),
            'fdLogging', FALSE,
            TRUE
          ),
          new IntAttribute (
            _('LDAP size limit'), _('Defines the number of entries to get from LDAP by default.'),
            'fdLdapSizeLimit', FALSE,
            0 /*min*/, FALSE /*no max*/, 200
          ),
        )
      ),
      'login' => array(
        'name'  => _('Login and session'),
        'attrs' => array(
          new SelectAttribute (
            _('Login attribute'),
            _('Which LDAP attribute should be used as the login name during login.'),
            'fdLoginAttribute', TRUE,
            array('uid', 'mail', 'uid,mail'), 'uid',
            array('uid', 'mail', 'both')
          ),
          new BooleanAttribute (
            _('Enforce encrypted connections'),
            _('Enables PHP security checks to force encrypted access (https) to the web interface.'),
            'fdForceSSL'
          ),
          new BooleanAttribute (
            _('Warn if session is not encrypted'),
            _('will display a warning to the user when http is used instead of https.'),
            'fdWarnSSL', FALSE,
            TRUE
          ),
          new BooleanAttribute (
            _('Remember dialog filter settings'),
            _('Store filter and plugin settings inside of a cookie.'),
            'fdStoreFilterSettings', FALSE,
            TRUE
          ),
          new IntAttribute (
            _('Session lifetime'), _('Defines when a session will expire in seconds.'),
            'fdSessionLifeTime', TRUE,
            0 /*min*/, FALSE /*no max*/, 1800
          ),
        )
      ),
      'snapshots' => array(
        'name'  => _('Snapshots / Undo'),
        'attrs' => array(
          new BooleanAttribute (
            _('Enable snapshots'), _('This enables you to save certain states of entries and restore them later on.'),
            'fdEnableSnapshots'
          ),
          new StringAttribute (
            _('Snapshot base'), _('The base where snapshots should be stored inside of the LDAP.'),
            'fdSnapshotBase'
          ),
          new StringAttribute (
            _('Snapshot server'), _('LDAP URI for the server which is used to do object snapshots.'),
            'fdSnapshotURI'
          ),
          new StringAttribute (
            _('Snapshot user'), _('The user which is used to authenticate when connecting to snapshot server.'),
            'fdSnapshotAdminDn'
          ),
          new PasswordAttribute (
            _('Snapshot password'), _('The credentials which are used in combination with snapshot user in order to authenticate.'),
            'fdSnapshotAdminPassword'
          ),
        )
      ),
      'miscellaneous' => array(
        'name'  => _('Miscellaneous'),
        'class' => array('fullwidth'),
        'attrs' => array(
          new OrderedArrayAttribute (
            new CompositeAttribute (
              _('Hooks that are called when specific actions happens'),
              'fdTabHook',
              array(
                new SelectAttribute(
                  _('tab'), _('The tab that this hook concerns'),
                  'hookTab', TRUE,
                  $plugins
                ),
                new SelectAttribute(
                  _('mode'), _('When to call this command'),
                  'hookMode', TRUE,
                  array('postcreate', 'postremove', 'postmodify',
                        'precreate', 'preremove', 'premodify')
                ),
                new StringAttribute(
                  _('cmd'), _('The command that will be called'),
                  'hookCmd', TRUE
                )
              ),
              '/^(.+)\\|(.+)\\|(.*)$/',
              '%s|%s|%s',
              '', // acl
              _('Hooks')
            ),
            FALSE, // non-ordered
            array(),
            TRUE // edition
          ),
          new SetAttribute(
            new StringAttribute (
              _('Available shells'), _('Available POSIX shells for FD users.'),
              'fdShells', FALSE
            ),
            array('/bin/ash','/bin/bash','/bin/csh','/bin/sh','/bin/ksh',
                  '/bin/tcsh','/bin/dash','/bin/zsh')
          ),
        )
      ),
      'people_and_group' => array(
        'name'  => _('People and group storage'),
        'class' => array('critical'),
        'icon'  => 'images/small_warning.png',
        'attrs' => array(
          new SelectAttribute (
            _('People DN attribute'), _('Attribute to use at the beginning of users dn'),
            'fdAccountPrimaryAttribute', TRUE,
            array('uid', 'cn')
          ),
          new BooleanAttribute (
            _('Include personal title in user DN'),
            _('Include personal title in user DN'),
            'fdPersonalTitleInDN'
          ),
          new StringAttribute (
            _('Customized CN'), _('Use a placeholder pattern for generating account cns'),
            'fdAccountRDN', FALSE
          ),
          new StringAttribute (
            _('Next id hook'), _('A script to be called for finding the next free id for users or groups externaly.'),
            'fdNextIdHook', FALSE
          ),
          new StringAttribute (
            _('Automatic UIDs'), _('Automatic way to generate new user ids'),
            'fdIdGenerator', FALSE
          ),
          new BooleanAttribute (
            _('Strict naming policy'),
            _('Enables strict checking of uids and group names'),
            'fdStrictNamingRules', FALSE,
            TRUE
          ),
          new IntAttribute (
            _('GID/UID min id'),
            _('The minimum assignable user or group id to avoid security leaks with uid 0 accounts.'),
            'fdMinId', FALSE,
            0, FALSE
          ),
          new IntAttribute (
            _('Number base for people'),
            _('Where to start looking for a new free user id.'),
            'fdUidNumberBase', FALSE,
            0, FALSE, 1100
          ),
          new IntAttribute (
            _('Number base for groups'),
            _('Where to start looking for a new free group id.'),
            'fdGidNumberBase', FALSE,
            0, FALSE, 1100
          ),
          new StringAttribute (
            _('Users RDN'), _('The branch where user are stored.'),
            'fdUserRDN', TRUE,
            'ou=people'
          ),
          new StringAttribute (
            _('Groups RDN'), _('The branch where groups are stored.'),
            'fdGroupRDN', TRUE,
            'ou=groups'
          ),
        )
      ),
      'debug' => array(
        'name'  => _('Debugging'),
        'attrs' => array(
          new BooleanAttribute (
            _('Display errors'),
            _('Shows PHP errors in the upper part of the screen. This should be disabled in productive deployments, because there might be some passwords in it.'),
            'fdDisplayErrors'
          ),
          new IntAttribute (
            _('Maximum LDAP query time'), _('Stop LDAP actions if there is no answer within the specified number of seconds.'),
            'fdLdapMaxQueryTime', FALSE,
            0 /*min*/, FALSE /*no max*/
          ),
          new BooleanAttribute (
            _('Log LDAP statistics'),
            _('Track LDAP timing statistics to the syslog. This may help to find indexing problems or bad search filters.'),
            'fdLdapStats'
          ),
          new DebugLevelAttribute (
            new SelectAttribute (
              _('Debug level'),
              _('Display certain information on each page load.'),
              'fdDebugLevel', FALSE,
              array(DEBUG_TRACE,  DEBUG_LDAP, DEBUG_DB,   DEBUG_SHELL,  DEBUG_POST,
                    DEBUG_SESSION,  DEBUG_ACL,  DEBUG_SI, DEBUG_MAIL),
                    DEBUG_TRACE,
              array('Trace',      'LDAP',     'Database', 'Shell',      'POST',
                    'SESSION',      'ACL',      'SI',     'Mail')
            )
          ),
        )
      ),
    );
  }

  function __construct(&$config, $dn = NULL, $object = NULL)
  {
    $attributesInfo = self::getAttributesInfo();
    /* Languages */
    $languages = get_languages(TRUE);
    asort($languages);
    $languages = array_merge(array("" => _("Automatic")), $languages);
    $attributesInfo['look_n_feel']['attrs'][0]->setChoices(array_keys($languages), array_values($languages));
    /* Timezones */
    $attributesInfo['look_n_feel']['attrs'][2]->setChoices(timezone::_get_tz_zones());
    /* Password methods */
    $methods = passwordMethod::get_available_methods();
    $methods = $methods['name'];
    $attributesInfo['password']['attrs'][0]->setChoices($methods);

    parent::__construct($config, $dn, $object, $attributesInfo);
    $this->attributesInfo['snapshots']['attrs']['fdEnableSnapshots']->setManagedAttributes(
      array(
        'disable' => array (
          FALSE => array (
            'fdSnapshotBase',
            'fdSnapshotURI',
            'fdSnapshotAdminDn',
            'fdSnapshotAdminPassword',
          )
        )
      )
    );
    $this->attributesInfo['people_and_group']['attrs']['fdAccountPrimaryAttribute']->setManagedAttributes(
      array(
        'erase' => array (
          'uid' => array (
            'fdPersonalTitleInDN',
          )
        )
      )
    );
    $this->attributesInfo['login']['attrs']['fdForceSSL']->setManagedAttributes(
      array(
        'disable' => array (
          TRUE => array (
            'fdWarnSSL',
          )
        )
      )
    );

  }

  static function get_themes()
  {
    $themesdir  = '../ihtml/themes/';
    $themes     = array();
    if ($dir = opendir("$themesdir")) {
      while (($file = readdir($dir)) !== FALSE) {
        if (is_dir("$themesdir/$file") && !preg_match("/^\./", $file)) {
          $themes[] = $file;
        }
      }
    }
    return $themes;
  }
}
?>
