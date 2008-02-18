<?php
// $Id$

/**
 * @file
 * Provides base classes for implementing facets, and classes needed by other
 * modules.
 */

/**
 * Base class for facet categories.
 */
class faceted_search_category {
  /**
   * The number of nodes associated to this category.
   */
  var $_count = NULL;

  /**
   * Constructor.
   *
   * @param $count
   *   The number of nodes associated to this category within the current
   *   search.
   */
  function faceted_search_category($count = NULL) {
    $this->_count = $count;
  }

  /**
   * Return the number of nodes associated to this category within the current
   * search.
   *
   * @return The number of matching nodes, or NULL is count is unknown.
   */
  function get_count() {
    return $this->_count;
  }

  /**
   * Updates a query for retrieving the subcategories of this category and their
   * associated nodes within the current search results. 
   *
   * This only needs to be overridden for hierarchical facets.
   *
   * @param $query
   *   The query object to update.
   *
   * @return
   *   FALSE if this facet can't have subcategories.
   */
  function build_subcategories_query(&$query) {
    return FALSE;
  }
}

/**
 * The base class for facets.
 */
class faceted_search_facet {
  
  /**
   * The key identifying this class of facet in search texts. Keys are used in
   * the form of 'key:text' tokens in search texts.
   */
  var $_key = '';

  /**
   * The status of this facet.
   */
  var $_status = FALSE;
  
  /**
   * The weight of this facet, for sorting purposes.
   */
  var $_weight = 0;

  /**
   * The current sort criteria to use for this facet. This determines how to
   * sort the facet's categories.
   */
  var $_sort = 'count';

  /**
   * The maximum number of categories to show in this facet.
   */
  var $_max_categories = 10;

  /**
   * An array representing the path of categories leading to the active category
   * of this facet (i.e. without the active category itself).
   */
  var $_path = array();
  
  /**
   * Constructor.
   *
   * @param $key
   *   Key corresponding to this class of facet. This should be the same string
   *   as used to construct the facet from the search text in the module's
   *   implementation of hook_faceted_search_parse(). 
   * @param $active_path
   *   Array representing the path leading to the active category, including the
   *   active category itself. Defaults to an empty array, meaning no active
   *   category.
   */
  function faceted_search_facet($key, $active_path = array()) {
    $this->_key = $key;
    $this->_path = $active_path;
  }

  /**
   * Assign settings to this facet.
   *
   * @param $settings
   *   Array of settings.
   */
  function set($settings) {
    if (isset($settings['status'])) {
      $this->_status = $settings['status'];
    }
    if (isset($settings['weight'])) {
      $this->_weight = $settings['weight'];
    }
    if (isset($settings['sort'])) {
      $this->_sort = $settings['sort'];
    }
    if (isset($settings['max_categories'])) {
      $this->_max_categories = $settings['max_categories'];
    }
  }
  
  /**
   * Return the key for this class of facet.
   */
  function get_key() {
    return $this->_key;
  }

  /**
   * Return a help text for site administrators.
   */
  function get_help() {
    return '';
  }
  
  /**
   * Return the status of this facet.
   *
   * @return
   *   TRUE when the facet is enabled, FALSE otherwise.
   */
  function get_status() {
    return $this->_status;
  }

  /**
   * Change the status of this facet.
   *
   * @param $status
   *   TRUE to enable the facet, FALSE to disable it.
   */
  function set_status($status) {
    $this->_status = $status;
  }
  
  /**
   * Return the configured weight of this facet, for sorting purposes.
   */
  function get_weight() {
    return $this->_weight;
  }

  /**
   * Assign the weight of this facet.
   */
  function set_weight($weight) {
    $this->_weight = $weight;
  }
  
  /**
   * Returns the available sort options for this facet. Each option is a key =>
   * label pair.
   *
   * Each key must have a corresponding handler method in the form
   * 'build_sort_query_key'.
   */
  function get_sort_options() {
    return array('count' => t('Count'));
  }

  /**
   * Returns the current sort criteria for this facet.
   */
  function get_sort() {
    return $this->_sort;
  }
  
  /**
   * Assigns the current sort criteria for this facet.
   */
  function set_sort($sort) {
    // Assign value only if a corresponding handler exists.
    if (method_exists($this, 'build_sort_query_'. $sort)) {
      $this->_sort = $sort;
    }
  }

  /**
   * Handler for the 'count' sort criteria.
   */
  function build_sort_query_count(&$query) {
    $query->add_orderby('count', 'DESC');
  }
  
  /**
   * Applies the facet's current sort option to the given query.
   */
  function build_sort_query(&$query) {
    $method = 'build_sort_query_'. $this->_sort;
    if (method_exists($this, $method)) {
      $this->$method($query);
    }
  }
  
  /**
   * Return the configured maximum number of categories to show in this facet.
   *
   * @return
   *   The maximum number of categories, or 0 for no limit.
   */
  function get_max_categories() {
    return $this->_max_categories;
  }

  /**
   * Assign the maximum number of categories to show in this facet.
   *
   * @param $max_categories
   *   The maximum number of categories, or 0 for no limit.
   */
  function set_max_categories($max_categories) {
    $this->_max_categories = $max_categories;
  }
  
  /**
   * Updates a query for retrieving the root categories of this facet and their
   * associated nodes within the current search results. 
   *
   * @param $query
   *   The query object to update.
   *
   * @return
   *   FALSE if this facet can't have root categories.
   */
  function build_root_categories_query() {
    return FALSE;
  }

  /**
   * This factory method creates categories given query results that include the
   * fields selected in get_root_categories_query() or get_subcategories_query().
   *
   * @param $results
   *   $results A database query result resource.
   *
   * @return
   *   Array of categories.
   */
  function build_categories($results) {
    return array();
  }
  
  /**
   * Return TRUE if this facet has an active category.
   */
  function is_active() {
    return count($this->_path) > 0;
  }

  /**
   * Return an array representing the path to the active category, including the
   * active category itself. Return an empty array if there is no active
   * category.
   */
  function get_active_path() {
    return $this->_path;
  }

  /**
   * Set the path of the active category, including the active category itself.
   *
   * @param $path
   *   The path of the category (array of categories). Defaults to no active
   *   path.
   */
  function set_active_path($path = array()) {
    $this->_path = $path;
  }

  /**
   * Return the active category, or NULL if there is no active category.
   */
  function get_active_category() {
    return end($this->_path);
  }
  
  /**
   * Replace the active category in the active path.
   *
   * The caller is responsible for making sure that the specified category is
   * consistent with the existing path (e.g. that the path contains actual
   * ancestors of that category).
   */
  function set_active_category($category) {
    $count = count($this->_path);
    if ($count > 0) {
      $this->_path[$count - 1] = $category;
    }
    else {
      $this->_path = array($category);
    }
  }
}

/**
 * A keyword category (for keyword facets).
 *
 * This is actually just a label and a search text, since keywords do not have
 * "subcategories", and queries for searching keywords are a special case
 * handled directly by faceted_search::execute().
 */
class faceted_search_keyword_category extends faceted_search_category {
  var $_label = '';
  var $_text = '';
  
  function faceted_search_keyword_category($label, $text) {
    $this->_label = $label;
    $this->_text = $text;
  }

  /**
   * Return the label of this category.
   *
   * @param $html
   *   TRUE when HTML is allowed in the label, FALSE otherwise. Checking this
   *   flag allows implementors to provide a rich-text label if desired, and an
   *   alternate plain text version for cases where HTML cannot be used. The
   *   implementor is responsible to ensure adequate security filtering.
   */
  function get_label($html = FALSE) {
    // Note: No need for calling check_plain() here, because the label has
    // already been checked by either of
    // theme_faceted_search_keyword_and_label(),
    // theme_faceted_search_keyword_or_label(),
    // theme_faceted_search_keyword_not_label, or
    // theme_faceted_search_keyword_phrase_label().
    if ($html) {
      return $this->_label;
    }
    else {
      // Some categories (e.g. OR keyword categories) may contain HTML tags.
      return strip_tags($this->_label);
    }
  }

  function get_text() {
    return $this->_text;
  }
}

/**
 * A keyword facet. This is the base class for other keyword facets.
 *
 * Keywords are not really facets, but giving them the same interface as a facet
 * helps display them in the current search block.
 */
class faceted_search_keyword_facet extends faceted_search_facet {
  
  function faceted_search_keyword_facet($active_category = NULL, $settings = array()) {
    parent::faceted_search_facet('keyword', $active_category ? array($active_category) : array(), $settings);
    parent::set_status(TRUE); // Keyword facets are always enabled.
  }

  /**
   * Returns the available sort options for this facet.
   */
  function get_sort_options() {
    return array(); // Sort options are not relevant to this kind of facet.
  }

  /**
   * Return the search text corresponding to this facet's active category.
   */
  function get_text() {
    if ($category = $this->get_active_category()) {
      return $category->get_text();
    }
    return '';
  }
}

/**
 * A keyword AND facet.
 */
class faceted_search_keyword_and_facet extends faceted_search_keyword_facet {

  function faceted_search_keyword_and_facet($keyword = '', $settings = array()) {
    if ($keyword) {
      $active_category = new faceted_search_keyword_category(theme('faceted_search_keyword_and_label', $keyword), $keyword);
    }
    parent::faceted_search_keyword_facet($active_category, $settings);
    parent::set_weight(-999); // Assign default weight.
  }

  function get_id() {
    return 'and';
  }

  function get_label() {
    return t('Keyword (AND)');
  }
}

/**
 * A keyword phrase facet.
 */
class faceted_search_keyword_phrase_facet extends faceted_search_keyword_facet {

  function faceted_search_keyword_phrase_facet($phrase = '', $settings = array()) {
    if ($phrase) {
      $active_category = new faceted_search_keyword_category(theme('faceted_search_keyword_phrase_label', $phrase), '"'. $phrase .'"');
    }
    parent::faceted_search_keyword_facet($active_category, $settings);
    parent::set_weight(-998); // Assign default weight.
  }

  function get_id() {
    return 'phrase';
  }

  function get_label() {
    return t('Keyword (Phrase)');
  }
}

/**
 * A keyword OR facet.
 */
class faceted_search_keyword_or_facet extends faceted_search_keyword_facet {
  
  function faceted_search_keyword_or_facet($keywords = array(), $settings = array()) {
    if (count($keywords) > 0) {
      $active_category = new faceted_search_keyword_category(theme('faceted_search_keyword_or_label', $keywords), implode(' OR ', $keywords));
    }
    parent::faceted_search_keyword_facet($active_category, $settings);
    parent::set_weight(-997); // Assign default weight.
  }

  function get_id() {
    return 'or';
  }

  function get_label() {
    return t('Keyword (OR)');
  }
}

/**
 * A keyword NOT facet.
 */
class faceted_search_keyword_not_facet extends faceted_search_keyword_facet {

  function faceted_search_keyword_not_facet($keyword = '', $settings = array()) {
    if ($keyword) {
      $active_category = new faceted_search_keyword_category(theme('faceted_search_keyword_not_label', $keyword), '-'. $keyword);
    }
    parent::faceted_search_keyword_facet($active_category, $settings);
    parent::set_weight(-996); // Assign default weight.
  }

  function get_id() {
    return 'not';
  }

  function get_label() {
    return t('Keyword (NOT)');
  }
}

/**
 * This class stores and processes data related to a search.
 */
class faceted_search {
  /**
   * The environment id for this search. Each search environment has its own
   * settings which make it possible to use multiple distinct search
   * interfaces. It is this id that allows to select the proper settings.
   */
  var $_env_id = 0;
  
  /**
   * The full, unprocessed search text.
   */
  var $_text = '';

  /**
   * The search keywords text.
   */
  var $_keywords = '';

  /**
   * Name of the temporary results table. While it exists, this table can be
   * queried for various purposes, such as building the search interface.
   */
  var $_results_table = '';

  /**
   * Total value of ranking weights. May be used only after a call to execute().
   */
  var $_results_total = 0;

  /**
   * Number of results in the results table. May be used only after a call to
   * execute().
   */
  var $_results_count = 0;

  /**
   * Flag to indicate whether the search has been executed.
   */
  var $_ready = FALSE;
  
  /**
   * Collection of facets currently used by this search.
   */
  var $_facets = array();

  /**
   * Constructor. Initialize the search data and parses the given search text.
   *
   * @param $env_id
   *   Id of the environment to use for this search.
   * @param $text
   *   Search text.
   */
  function faceted_search($env_id, $text = '') {
    $this->_env_id = $env_id;
    $this->_text = $text;
    $this->_results_table = 'temp_faceted_search_results_'. $env_id;

    // Load settings for all enabled facets in this search environment.
    $all_settings = faceted_search_load_facet_settings($env_id);
    
    // Make a filter with all enabled facets.
    $filter = faceted_search_get_facet_filter($all_settings);

    // Collect all facets relevant to this search.
    foreach (module_implements('faceted_search_collect') as $module) {
      $module_facets = array();
      $hook = $module .'_faceted_search_collect';

      // Parse the search text and obtain corresponding facets. Text is stripped
      // from facet data so the remaining text is considered as simple keywords.
      $text = $hook($module_facets, 'text', $env_id, $filter, $text);

      // Disallow facets that have been collected from the search text.
      foreach ($module_facets as $facet) {
        unset($filter[$facet->get_key()][$facet->get_id()]);
      }
      
      // Obtain remaining allowed facets.
      if (!empty($filter)) {
        $hook($module_facets, 'all', $env_id, $filter);
      }
      
      // Merge the facets listed by the current module.
      $this->_facets = array_merge($this->_facets, $module_facets);

      if (empty($filter)) {
        break; // No more facets allowed.
      }
    }
    
    // Assign the remaining text as keywords.
    $this->_keywords = $text;

    // Keyword pseudo-facets must be created after all the other facets so that
    // no recognized facet token gets interpreted as a keyword. Keyword facets
    // also don't need a filter since they are always enabled.
    faceted_search_collect_keyword_facets($this->_facets, 'text', $env_id, $text);
    
    // Prepare facets for use, assigning them their settings are sorting them.
    faceted_search_prepare_facets($this->_facets, $all_settings);
  }

  /**
   * Return the environment id of this search.
   */
  function get_env_id() {
    return $this->_env_id;
  }
  
  /**
   * Return the original search text of this search (i.e. the text that was
   * passed to the constructor).
   */
  function get_text() {
    return $this->_text;
  }

  /**
   * Return the search keywords (the search text after having extracted the
   * facets).
   */
  function get_keywords() {
    return $this->_keywords;
  }
  
  /**
   * Return the facets used by this search.
   */
  function get_facets() {
    return $this->_facets;
  }

  /**
   * Return the specified facet.
   */
  function get_facet($index) {
    return $this->_facets[$index];
  }

  /** 
   * Return the index of a facet given its key and id.
   */
  function get_facet_by_id($key, $id) {
    foreach ($this->_facets as $index => $facet) {
      if ($facet->get_key() == $key && $facet->get_id() == $id) {
        return array($index, $facet);
      }
    }
  }

  /**
   * Return TRUE when the search has been executed.
   */
  function ready() {
    return $this->_ready;
  }
  
  /**
   * Perform the search and store the results in a temporary table.
   *
   * Two queries are performed. The first query selects a set of possible
   * matches based on the search index and the given keywords. This is the
   * classic "OR" search.
   *
   * The second query further refines this set by verifying advanced conditions
   * (such as filters, facets, AND, negative or phrase matches), and orders the
   * results by score.
   */
  function execute() {
    if (!$this->_keywords && !$this->_facets) {
      return; // Nothing to search
    }
    
    // Build first query
    $query1 = new faceted_search_query;
    $query1->add_where("n.status = 1");
    
    $query_parts = search_parse_query($this->_keywords);
    if ($query_parts[2] && $query_parts[3]) {
      $args = array_values($query_parts[3]);
      array_unshift($args, $query_parts[2]); // Pass the $clause arg to add_where().
      call_user_func_array(array(&$query1, 'add_where'), $args); // Pass all args to add_where().
    }
    
    if ($query_parts[0] || $query_parts[2]) {
      // Has some positive or negative keywords
      $query1->add_table('search_index', 'sid', 'n', 'nid', 'i');
      $query1->add_table('search_total', 'word', 'i', 'word', 't');
      $query1->add_where("i.type = 'node'");
      $query1->add_field(NULL, 'SUM(i.score * t.count)', 'relevance');
      $query1->add_field(NULL, 'COUNT(*)', 'matches');
    }
    else {
      // Has no keywords
      $query1->add_field(NULL, '1', 'relevance');
      $query1->add_field(NULL, 'COUNT(*)', 'matches');
    }
    $query1->add_groupby('nid');
    $query1->add_having('COUNT(*) >= %d', (int)$query_parts[4]);
    
    // Build second query
    $query2 = new faceted_search_query;
    
    // Based on node_search() - BEGIN
    // Build ranking expression (we try to map each parameter to a
    // uniform distribution in the range 0..1).
    
    // Always join the table resulting from $query1.
    $query2->add_table('temp_faceted_search_nids_'. $this->_env_id, 'nid', 'n', 'nid', 'i', 'INNER', FALSE);
    if ($query_parts[0] || $query_parts[2]) {
      // Has some positive or negative keywords
      $query2->add_table('search_dataset', 'sid', 'n', 'nid', 'd');
      $query2->add_where("d.type = 'node'");
    }

    // Apply node type filter
    if ($types = faceted_search_types($this->_env_id)) {
      $query2->add_where("n.type IN ('". implode("','", $types) ."')");
    }
    // Used to avoid joining on node_comment_statistics twice
    $stats_added = FALSE;
    $ranking = array();
    $this->_results_total = 0;
    if ($weight = (int)variable_get('node_rank_relevance', 5)) {
      // Average relevance values hover around 0.15
      $ranking[] = sprintf('%d * i.relevance', $weight);
      $this->_results_total += $weight;
    }
    if ($weight = (int)variable_get('node_rank_recent', 5)) {
      // Exponential decay with half-life of 6 months, starting at last indexed node
      $ranking[] = sprintf('%d * POW(2, (GREATEST(n.created, n.changed, IFNULL(c.last_comment_timestamp, 0)) - %d) * 6.43e-8)', $weight, (int)variable_get('node_cron_last', 0)); // TODO: see http://drupal.org/node/106659
      $query2->add_table('node_comment_statistics', 'nid', 'n', 'nid', 'c', 'LEFT');
      $stats_added = TRUE;
      $this->_results_total += $weight;
    }
    if (module_exists('comment') && $weight = (int)variable_get('node_rank_comments', 5)) {
      // Inverse law that maps the highest reply count on the site to 1 and 0 to 0.
      $scale = variable_get('node_cron_comments_scale', 0.0);
      $ranking[] = sprintf('%d * (2.0 - 2.0 / (1.0 + IFNULL(c.comment_count, 0) * %f))', $weight, $scale);
      if (!$stats_added) {
        $query2->add_table('node_comment_statistics', 'nid', 'n', 'nid', 'c', 'LEFT');
      }
      $this->_results_total += $weight;
    }
    if (module_exists('statistics') && variable_get('statistics_count_content_views', 0) && $weight = (int)variable_get('node_rank_views', 5)) {
      // Inverse law that maps the highest view count on the site to 1 and 0 to 0.
      $scale = variable_get('node_cron_views_scale', 0.0);
      $ranking[] = sprintf('%d * (2.0 - 2.0 / (1.0 + IFNULL(nc.totalcount, 0) * %f))', $weight, $scale);
      $query2->add_table('node_counter', 'nid', 'n', 'nid', 'nc', 'LEFT');
      $this->_results_total += $weight;
    }
    if (count($ranking)) {
      $query2->add_field(NULL, '(('. implode(') + (', $ranking) .')) / '. $this->_results_total, 'score');
    }
    else {
      $query2->add_field(NULL, 'i.relevance / '. $this->_results_total, 'score');
    }
    // Based on node_search() - END
    if ($query_parts[0] && $query_parts[1]) {
      $args = array_values($query_parts[1]);
      array_unshift($args, $query_parts[0]); // Pass the $clause arg to add_where().
      call_user_func_array(array(&$query2, 'add_where'), $args); // Pass all args to add_where().
    }
    $query2->add_orderby('score', 'DESC');

    // Apply facets into query, except for keywords (which are already included
    // in the query)
    foreach ($this->_facets as $facet) {
      if ($facet->get_key() != 'keyword') {
        if ($category = $facet->get_active_category()) {
          $category->build_results_query($query2);
        }
      }
    }

    // Give other modules an opportunity at altering the final query (e.g. for
    // additional filtering).
    module_invoke_all('faceted_search_query_alter', $this, $query2);
    
    // Perform the search.
    $this->_execute_search_query($query1, $query2);
    $this->_ready = TRUE; 
  }

  /**
   * Fetch the items from the current search results, or from all available
   * nodes if no search text has been given.
   *
   * execute() must have been called beforehand.
   *
   * @return
   *   Array of objects with nid and score members. 
   */
  function load_results($limit = 10) {
    $found_items = array();
    if ($this->_results_count) {
      $result = pager_query("SELECT * FROM ". $this->_results_table, $limit, 0, 'SELECT '. $this->_results_count);
      while ($item = db_fetch_object($result)) {
        $found_items[] = $item;
      }
    }
    return $found_items;
  }
  
  /**
   * Return the number of results for this search.
   *
   * execute() must have been called beforehand.
   */
  function get_results_count() {
    return $this->_results_count;
  }

  /**
   * Return the name of this search's (temporary) results table.
   */
  function get_results_table() {
    return $this->_results_table;
  }
  
  /**
   * Return the categories for the given facet and count matching nodes within
   * results.
   *
   * @param $facet
   *   The facet whose categories are to be loaded.
   *
   * @param $from
   *   Ordinal number of the first category to load. Numbering starts at 0.
   *
   * @param $max_count
   *   Number of categories to load.
   *
   * @return
   *   Array of categories (objects having the faceted_search_category
   *   interface).
   */
  function load_categories($facet, $from = NULL, $max_count = NULL) {
    // Prepare the base query components to include the current search results
    // and to count nodes.
    $query = new faceted_search_query;
    $query->add_field(NULL, 'COUNT(DISTINCT(n.nid))', 'count');
    if (!$this->_ready) {
      // No temporary table available, search within all nodes.
      $query->add_where('n.status = 1');

      // Apply node type filter.
      if ($types = faceted_search_types($this->_env_id)) {
        $query->add_where("n.type IN ('". implode("','", $types) ."')");
      }
      
      // There is no results table at this point, so we can't rely on the
      // results table having been filtered already. Therefore, we ask modules
      // to alter the categories query instead.
      module_invoke_all('faceted_search_query_alter', $this, $query);
    }
    elseif ($this->_results_count > 0) {
      // Search within results.
      $query->add_table($this->_results_table, 'nid', 'n', 'nid', 'results', 'INNER', FALSE);
    }
    else {
      // Current search yields no results, thus no categories are possible.
      return array(); 
    }

    // Gather the query components that will retrieve the categories.
    if ($active_category = $facet->get_active_category()) {
      $has_categories = $active_category->build_subcategories_query($query);
    }
    else {
      $has_categories = $facet->build_root_categories_query($query);
    }
    if (!$has_categories) {
      return array();
    }
    
    // Apply sort criteria.
    $facet->build_sort_query($query);
    
    // Apply node type filter.
    if ($types = faceted_search_types($this->_env_id)) {
      $query->add_where("n.type IN ('". implode("','", $types) ."')");
    }
    
    // Run the query and return the categories.
    if (isset($from) && isset($max_count)) {
      $results = db_query_range($query->query(), $query->args(), $from, $max_count);
    }
    else {
      $results = db_query($query->query(), $query->args());
    }
    return $facet->build_categories($results);
  }

  /**
   * Run the specified queries and store results in the temporary table.
   */
  function _execute_search_query($query1, $query2) {
    // This is similar to search.module's do_search()

    // Execute query1
    db_query_temporary($query1->query(), $query1->args(), 'temp_faceted_search_nids_'. $this->_env_id);

    // Calculate maximum relevance and normalize it.
    $normalize = db_result(db_query('SELECT MAX(relevance) FROM temp_faceted_search_nids_'. $this->_env_id));
    if ($normalize) {
      foreach ($query2->fields as $alias => $field) {
        $query2->fields[$alias] = str_replace('i.relevance', '('. (1.0 / $normalize) .' * i.relevance)', $field);
      }
    }
    
    // Execute query2
    db_query_temporary($query2->query(), $query2->args(), $this->_results_table);
    $this->_results_count = db_result(db_query('SELECT COUNT(*) FROM '. $this->_results_table));
  }
}

/**
 * This class allows to build queries piece by piece.
 */
class faceted_search_query {
  var $primary_table_alias = '';
  var $primary_field_alias = '';
  var $table_queue = array(); // Ordered array of tables aliases to join.
  var $tables = array(); // Tables to join, keyed by their alias.
  var $fields = array(); // Fields, keyed by their alias.
  var $groupby = array();
  var $having = array();
  var $having_args = array();
  var $orderby = array();
  var $where = array();
  var $where_args = array();
  var $subqueries = array();
  var $subqueries_args = array();

  /**
   * Constructor. Specifies the primary table and field for this query.
   */
  function faceted_search_query($primary_table = 'node', $primary_field = 'nid', $primary_table_alias = 'n', $primary_field_alias = 'nid', $prefixing = TRUE) {
    $this->primary_table_alias = $primary_table_alias;
    $this->primary_field_alias = $primary_field_alias;
    $this->tables[$primary_table_alias] = array(
      'table' => $primary_table,
      'field' => NULL,
      'left_table_alias' => NULL,
      'left_field' => NULL,
      'join' => NULL,
      'prefixing' => $prefixing,
    );
    $this->fields[$primary_field_alias] = array(
      'field' => $primary_field,
      'table_alias' => $primary_table_alias,
    );
  }

  /**
   * Adds a table to join.
   *
   * @param $table
   *   Name of the table to join.
   *
   * @param $field
   *   Field to use in the ON condition of the join clause.
   *
   * @param $left_table_alias
   *   Alias of the table to use on the left part of the join. That table must
   *   be the query's primary table or another table added through
   *   add_table(). This must be an alias as returned by add_table(). If
   *   omitted, the primary table is used.
   *
   * @param $left_field
   *   Field from the left table to use in the ON condition of the join
   *   clause. If omitted, the primary field is used.
   *
   * @param $alias
   *   Alias to use for the table being added. If unspecified, the alias will be
   *   the same as the table's name. A unique alias must be given if the table
   *   is to be joined multiple times.
   *
   * @param $join
   *   Type of join clause to use. Default is 'INNER'.
   *
   * @param $prefixing
   *   TRUE when the table should be prefixed via db_prefix_tables(). This
   *   should usually be FALSE when joining a temporary table.
   *
   * @return
   *   The alias assigned to the table in this query.
   */
  function add_table($table, $field = NULL, $left_table_alias = NULL, $left_field = NULL, $alias = NULL, $join = 'INNER', $prefixing = TRUE) {
    $alias = $alias ? $alias : $table;
    $this->table_queue[] = $alias;
    $this->tables[$alias] = array(
      'table' => $table,
      'field' => $field ? $field : $this->fields[$this->primary_field_alias]['field'],
      'left_table_alias' => $left_table_alias ? $left_table_alias : $this->primary_table_alias,
      'left_field' => $left_field ? $left_field : $this->fields[$this->primary_field_alias]['field'],
      'join' => $join,
      'prefixing' => $prefixing,
    );
    return $alias;
  }

  /**
   * Adds a field.
   *
   * @param $table_alias
   *   Alias of the table containing the field, either the primary table or an
   *   alias returned by add_table(). Use NULL for a formula.
   *
   * @param $field
   *   The name of the field, or the formula defining the field.
   *
   * @param $alias
   *   Alias to use to identify the field. If omitted, the alias will be
   *   $table_alias .'_'. $field. Must be specified if the field is a formula.
   *
   * @return
   *   The alias assigned to the field in this query.
   */
  function add_field($table_alias, $field, $alias = NULL) {
    $alias = $alias ? $alias : $table_alias .'_'. $field;
    $this->fields[$alias] = array(
      'field' => $field,
      'table_alias' => $table_alias,
    );
    return $alias;
  }

  /**
   * Adds a WHERE condition.
   *
   * @param $clause
   *   The condition to add. The caller must ensure that any field is fully
   *   qualified using its table's alias as returned by add_table().
   *
   * @param ...
   *   A variable number of arguments which are substituted into the query using
   *   printf) syntax. The query arguments can be enclosed in one array
   *   instead. Valid %-modifiers are: %s, %d, %f, %b (binary data, do not
   *   enclose in '') and %%.
   */
  function add_where($clause) {
    $this->where[] = $clause;
    $args = func_get_args();
    array_shift($args); // Skip $clause.
    if (count($args)) {
      $this->where_args = array_merge($this->where_args, $args);
    }
  }

  /**
   * Adds a subquery as a WHERE condition.
   *
   * @param $clause
   *   The condition to add. The caller must ensure that any field is fully
   *   qualified using its table's alias as returned by add_table(). The caller
   *   is responsible for calling db_rewrite_sql() on the subquery.
   *
   * @param ...
   *   A variable number of arguments which are substituted into the query using
   *   printf) syntax. The query arguments can be enclosed in one array
   *   instead. Valid %-modifiers are: %s, %d, %f, %b (binary data, do not
   *   enclose in '') and %%.
   */
  function add_subquery($clause) {
    $this->subqueries[] = $clause;
    $args = func_get_args();
    array_shift($args); // Skip $clause.
    if (count($args)) {
      $this->subqueries_args = array_merge($this->subqueries_args, $args);
    }
  }
  
  /**
   * Adds a GROUP BY clause.
   *
   * @param $clause
   *   The clause to add. The caller must use field aliases as returned by
   *   add_field().
   *
   * @param $order
   *   Either 'ASC' or 'DESC'.
   */
  function add_groupby($clause, $order = 'ASC') {
    $this->groupby[] = $clause .' '. $order;
  }

  /**
   * Adds a HAVING clause.
   *
   * @param $clause
   *   The clause to add. The caller must ensure that any field is fully
   *   qualified using its table's alias as returned by add_table().
   *
   * @param ...
   *   A variable number of arguments which are substituted into the query using
   *   printf) syntax. The query arguments can be enclosed in one array
   *   instead. Valid %-modifiers are: %s, %d, %f, %b (binary data, do not
   *   enclose in '') and %%.
   */
  function add_having($clause) {
    $this->having[] = $clause;
    $args = func_get_args();
    array_shift($args); // Skip $clause.
    if (count($args)) {
      $this->having_args = array_merge($this->having_args, $args);
    }
  }

  /**
   * Adds an ORDER BY clause.
   *
   * @param $clause
   *   The clause to add. The caller must use field aliases as returned by
   *   add_field().
   *
   * @param $order
   *   Either 'ASC' or 'DESC'.
   */
  function add_orderby($clause, $order = 'ASC') {
    $this->orderby[] = $clause .' '. $order;
  }

  /**
   * Returns all arguments that need to be substituted into the query.
   */
  function args() {
    return array_merge($this->where_args, $this->subqueries_args, $this->having_args);
  }

  /**
   * Returns the SQL query (with unsubstituted arguments, if any).
   *
   * @see args
   */
  function query() {
    // Fields.
    $fields = array();
    foreach ($this->fields as $field_alias => $field) {
      if ($field['table_alias']) {
        $table = $field['table_alias'];
        $fields[] = "$table.$field[field] AS $field_alias";
      }
      else {
        $fields[] = "$field[field] AS $field_alias";
      }
    }
    $fields = implode(', ', $fields);
    if ($this->tables[$this->primary_table_alias]['prefixing']) {
      $wrapper_begin = '{';
      $wrapper_end = '}';
    }
    else {
      $wrapper_begin = $wrapper_end = '';
    }
    $primary = $wrapper_begin . $this->tables[$this->primary_table_alias]['table'] . $wrapper_end .' AS '. $this->primary_table_alias;

    // Joins.
    $joins = array();
    foreach ($this->table_queue as $table_alias) {
      $table = $this->tables[$table_alias];
      if ($table['prefixing']) {
        $wrapper_begin = '{';
        $wrapper_end = '}';
      }
      else {
        $wrapper_begin = $wrapper_end = '';
      }
      $joins[] = "$table[join] JOIN $wrapper_begin". $table['table'] ."$wrapper_end AS $table_alias ON $table[left_table_alias].$table[left_field] = $table_alias.$table[field]";
    }
    $joins = count($joins) ? ' '. implode(' ', $joins) : '';

    // Where clauses.
    $where = count($this->where) ? ' WHERE (('. implode(') AND (', $this->where) .'))' : '';

    // Where subqueries (added as $SUBQUERY$n tokens, which are replaced after the call
    // to db_rewrite_sql(). See related issue: http://drupal.org/node/151910).
    if (count($this->subqueries)) {
      $subqueries = $where == '' ? ' WHERE ' : ' AND ';
      $subqueries .= '$SUBQUERY$'. implode(' AND $SUBQUERY$', array_keys($this->subqueries));
    }
      
    // Group by clauses.
    $groupby = count($this->groupby) ? ' GROUP BY '. implode(', ', $this->groupby) : '';

    // Having clauses.
    $having = count($this->having) ? ' HAVING (('. implode(') AND (', $this->having) .'))' : '';

    // Order by clauses.
    $orderby = count($this->orderby) ? ' ORDER BY '. implode(', ', $this->orderby) : '';
    
    // Create the query string.
    $query = db_rewrite_sql("SELECT $fields FROM $primary$joins$where$subqueries$groupby$having$orderby");
    if (count($this->subqueries)) {
      foreach ($this->subqueries as $key => $subquery) {
        $search[] = '$SUBQUERY$'. $key;
        $replace[] = $subquery;
      }
      // Replace subquery tokens.
      $query = str_replace($search, $replace, $query);
    }
    
    return $query;
  }
}

