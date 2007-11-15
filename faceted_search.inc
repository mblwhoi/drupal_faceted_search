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
   * The weight of this facet, for sorting purposes.
   */
  var $_weight = 0;

  /**
   * The current sort criteria to use for this facet. This determines how to
   * sort the facet's categories.
   */
  var $_sort = '';

  /**
   * The maximum number of categories to show in this facet.
   */
  var $_max_categories = 0;

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
   *
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
   * Assign this facet's settings.
   *
   * @param $module
   *   Creator module of this facet.
   */
  function load_settings($module) {
    // Retrieve the settings for this facet
    $settings = variable_get('faceted_search_display', array());
    $key = $module .'_'. $this->get_id();
    
    // Assign this facet's settings
    $this->_weight = isset($settings[$key]['weight']) ? $settings[$key]['weight'] : $this->get_default_weight();
    $this->_sort = isset($settings[$key]['sort']) ? $settings[$key]['sort'] : $this->get_default_sort();
    $this->_max_categories = isset($settings[$key]['max']) ? $settings[$key]['max'] : $this->get_default_max_categories();
  }

  /**
   * Return the key for this class of facet.
   */
  function get_key() {
    return $this->_key;
  }

  /**
   * Return the configured weight of this facet, for sorting purposes.
   */
  function get_weight() {
    return $this->_weight;
  }

  /**
   * Return the default weight of this facet.
   *
   * Subclasses may want to override this in order to use the some weight
   * associated to the data underlying the facet.
   */
  function get_default_weight() {
    return 0;
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
   * Returns the default sort criteria for this facet.
   */
  function get_default_sort() {
    return 'count';
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
   *   Maximum number of categories, or 0 for no limit.
   */
  function get_max_categories() {
    return $this->_max_categories;
  }

  /**
   * Return the default maximum number of categories to show in this facet.
   *
   * Subclasses may want to override this.
   */
  function get_default_max_categories() {
    return 10;
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

  function get_label() {
    return $this->_label;
  }

  function get_text() {
    return $this->_text;
  }
}

/**
 * A keyword facet. This is the base class for other keyword facets.
 */
class faceted_search_keyword_facet extends faceted_search_facet {
  
  function faceted_search_keyword_facet($active_category = NULL) {
    parent::faceted_search_facet('keyword', $active_category ? array($active_category) : array());
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

  function faceted_search_keyword_and_facet($keyword = '') {
    if ($keyword) {
      $active_category = new faceted_search_keyword_category(theme('faceted_search_keyword_and_label', $keyword), $keyword);
    }
    parent::faceted_search_keyword_facet($active_category);
  }

  function get_id() {
    return 'and';
  }

  function get_label() {
    return t('Keyword (AND)');
  }
  
  function get_default_weight() {
    return -999;
  }
  
}

/**
 * A keyword phrase facet.
 */
class faceted_search_keyword_phrase_facet extends faceted_search_keyword_facet {

  function faceted_search_keyword_phrase_facet($phrase = '') {
    if ($phrase) {
      $active_category = new faceted_search_keyword_category(theme('faceted_search_keyword_phrase_label', $phrase), '"'. $phrase .'"');
    }
    parent::faceted_search_keyword_facet($active_category);
  }

  function get_id() {
    return 'phrase';
  }

  function get_label() {
    return t('Keyword (Phrase)');
  }
  
  function get_default_weight() {
    return -998;
  }
}

/**
 * A keyword OR facet.
 */
class faceted_search_keyword_or_facet extends faceted_search_keyword_facet {
  
  function faceted_search_keyword_or_facet($keywords = array()) {
    if (count($keywords) > 0) {
      $active_category = new faceted_search_keyword_category(theme('faceted_search_keyword_or_label', $keywords), implode(' OR ', $keywords));
    }
    parent::faceted_search_keyword_facet($active_category);
  }

  function get_id() {
    return 'or';
  }

  function get_label() {
    return t('Keyword (OR)');
  }
  
  function get_default_weight() {
    return -997;
  }
}

/**
 * A keyword NOT facet.
 */
class faceted_search_keyword_not_facet extends faceted_search_keyword_facet {

  function faceted_search_keyword_not_facet($keyword = '') {
    if ($keyword) {
      $active_category = new faceted_search_keyword_category(theme('faceted_search_keyword_not_label', $keyword), '-'. $keyword);
    }
    parent::faceted_search_keyword_facet($active_category);
  }

  function get_id() {
    return 'not';
  }

  function get_label() {
    return t('Keyword (NOT)');
  }
  
  function get_default_weight() {
    return -996;
  }
}

/**
 * This class stores and processes data related to a search.
 */
class faceted_search {
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
   * re-used for other purposes, such as building the search interface.
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
   * Collection of facets currently used by this search.
   */
  var $_facets = array();

  /**
   * Constructor. Initialize the search data and parses the given search text.
   *
   * @param $text
   *   Search text.
   *
   * @param $table
   *   Table with search results. This must be specified if more than a single
   *   faceted search has to be executed.
   */
  function faceted_search($text, $table = 'temp_faceted_search_results') {
    $this->_text = $this->_keywords = $text;
    $this->_results_table = $table;

    // Will parse the given text into facets, stripping parsed text from the
    // search text and leaving only the keywords that do not match any facet.
    $this->_parse_facets();

    // Parse the remaining keywords and create corresponding "keyword" facets.
    $this->_parse_keywords();

    uasort($this->_facets, '_faceted_search_compare_facets');
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
      return array(); // Nothing to search
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
    $query2->add_table('temp_faceted_search_nids', 'nid', 'n', 'nid', 'i', 'INNER', FALSE);
    if ($query_parts[0] || $query_parts[2]) {
      // Has some positive or negative keywords
      $query2->add_table('search_dataset', 'sid', 'n', 'nid', 'd');
    }

    // Apply node type filter
    if ($excluded_types = faceted_search_excluded_types()) {
      $query2->add_where("n.type NOT IN ('". implode("','", $excluded_types) ."')");
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
    
    // Perform search
    $this->_execute_search_query($query1, $query2);
  }

  /**
   * Return the formatted results.
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
   */
  function load_categories($facet, $from = NULL, $max_count = NULL) {
    // Prepare the base query components to include the current search results
    // and to count nodes.
    $query = new faceted_search_query;
    $query->add_field(NULL, 'COUNT(n.nid)', 'count');
    if ($this->_results_count > 0) {
      // Search within results.
      $query->add_table($this->_results_table, 'nid', 'n', 'nid', 'results', 'INNER', FALSE);
    }
    elseif (empty($this->_text)) {
      // No search text, search within all nodes.
      $query->add_where('n.status = 1');
    }
    else {
      // There is a search text but the results count is 0, thus no categories
      // are possible!
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
    if ($excluded_types = faceted_search_excluded_types()) {
      $query->add_where("n.type NOT IN ('". implode("','", $excluded_types) ."')");
    }
    
    // Run the query and return the categories.
    if (isset($from) && isset($max_count)) {
      $results = db_query_range(db_rewrite_sql($query->query()), $query->args(), $from, $max_count);
    }
    else {
      $results = db_query(db_rewrite_sql($query->query()), $query->args());
    }
    return $facet->build_categories($results);
  }
  
  /**
   * Add a facet to this search.
   *
   * If the specified facet has an active category, it will replace any existing
   * facet with the same id that has no active category.
   *
   * If the specified facet has no active category, it will be added only if no
   * facet exists with the same id.
   *
   * @param $facet
   *   Facet to add.
   *
   * @param $module
   *   Creator module of the facet.
   */
  function _add_facet($facet, $module) {
    $facet->load_settings($module);
    $this->_facets[] = $facet;
  }
  
  /**
   * Helper function to invoke 'faceted_search_parse' on other modules, passing
   * on the keywords search text as reference (so they can modify them).
   */
  function _parse_facets() {
    foreach (module_implements('faceted_search_list') as $module) {
      // Retrieve all available facets from the module
      $available_facets = _faceted_search_list($module);

      // Ask the module to parse keywords and create corresponding facets
      $used_facets = array();
      if (module_hook($module, 'faceted_search_parse')) {
        $function = $module .'_faceted_search_parse';
        $used_facets = $function($this->_keywords);
      }

      foreach ($used_facets as $facet) {
        // Keep only those available facets that are not used by the search
        if (isset($available_facets[$facet->get_id()])) {
          unset($available_facets[$facet->get_id()]);
        }

        // Add the used facet into this search
        $this->_add_facet($facet, $module);
      }

      // Add the remaining available facets
      foreach ($available_facets as $facet) {
        // Add the used facet into this search
        $this->_add_facet($facet, $module);
      }
    }
  }
  
  /**
   * Helper function for parsing keywords and classify them into 'positive' and
   * 'negative' arrays.
   *
   * @return Array with subarrays 'positive' and 'negative'.
   */
  function _parse_keywords() {
    // TODO: Most of this function was taken from search_parse_query(), which is
    // called in faceted_search::execute(), which means this processing is
    // done twice. Needs to be fixed. Splitting search_parse_query() into two
    // distinct functions could help.
    $keys = array('positive' => array(), 'negative' => array());

    // Tokenize query string
    $matches = array();
    preg_match_all('/ (-?)("[^"]+"|[^" ]+)/i', ' '. $this->_keywords, $matches, PREG_SET_ORDER);

    // Taken from search_parse_query() (search.module 1.207) - BEGIN
    // Classify tokens
    $or = FALSE;
    foreach ($matches as $match) {
      $phrase = FALSE;
      // Strip off phrase quotes
      if ($match[2]{0} == '"') {
        $match[2] = substr($match[2], 1, -1);
        $phrase = TRUE;
      }
      // Simplify keyword according to indexing rules and external preprocessors
      $words = search_simplify($match[2]);
      // Re-explode in case simplification added more words, except when matching a phrase
      $words = $phrase ? array($words) : preg_split('/ /', $words, -1, PREG_SPLIT_NO_EMPTY);
      // Negative matches
      if ($match[1] == '-') {
        $keys['negative'] = array_merge($keys['negative'], $words);
      }
      // OR operator: instead of a single keyword, we store an array of all
      // OR'd keywords.
      elseif ($match[2] == 'OR' && count($keys['positive'])) {
        $last = array_pop($keys['positive']);
        // Starting a new OR?
        if (!is_array($last)) {
          $last = array($last);
        }
        $keys['positive'][] = $last;
        $or = TRUE;
        continue;
      }
      // Plain keyword
      else {
        if ($or) {
          // Add to last element (which is an array)
          $keys['positive'][count($keys['positive']) - 1] = array_merge($keys['positive'][count($keys['positive']) - 1], $words);
        }
        else {
          $keys['positive'] = array_merge($keys['positive'], $words);
        }
      }
      $or = FALSE;
    }
    // Taken from search_parse_query() - END

    // Create the facets
    foreach ($keys['positive'] as $keyword) {
      if (is_array($keyword)) {
        $this->_add_facet(new faceted_search_keyword_or_facet($keyword), 'faceted_search');
      }
      elseif (strpos($keyword, ' ')) {
        $this->_add_facet(new faceted_search_keyword_phrase_facet($keyword), 'faceted_search');
      }
      else {
        $this->_add_facet(new faceted_search_keyword_and_facet($keyword), 'faceted_search');
      }
    }
    foreach ($keys['negative'] as $keyword) {
      $this->_add_facet(new faceted_search_keyword_not_facet($keyword), 'faceted_search');
    }
  }

  /**
   * Run the specified queries and store results in the temporary table.
   */
  function _execute_search_query($query1, $query2) {
    // This is similar to search.module's do_search()

    // Drop query1's temporary table if it exists -- this allows for multiple
    // faceted searches per page request
    db_query('DROP TEMPORARY TABLE IF EXISTS temp_faceted_search_nids'); 
    
    // Execute query1
    db_query_temporary(db_rewrite_sql($query1->query()), $query1->args(), 'temp_faceted_search_nids');

    // Calculate maximum relevance and normalize it.
    $normalize = db_result(db_query('SELECT MAX(relevance) FROM temp_faceted_search_nids'));
    if (!$normalize) {
      return array();
    }
    foreach ($query2->fields as $alias => $field) {
      $query2->fields[$alias] = str_replace('i.relevance', '('. (1.0 / $normalize) .' * i.relevance)', $field);
    }

    // Execute query2
    db_query_temporary(db_rewrite_sql($query2->query()), $query2->args(), $this->_results_table);
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
    return array_merge($this->where_args, $this->having_args);
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

    // Group by clauses.
    $groupby = count($this->groupby) ? ' GROUP BY '. implode(', ', $this->groupby) : '';

    // Having clauses.
    $having = count($this->having) ? ' HAVING (('. implode(') AND (', $this->having) .'))' : '';

    // Order by clauses.
    $orderby = count($this->orderby) ? ' ORDER BY '. implode(', ', $this->orderby) : '';
    
    // Create the query string.
    return "SELECT $fields FROM $primary$joins$where$groupby$having$orderby";
  }
}

/**
 * Invoke hook_faceted_search_list() on the specified module.
 *
 * @return
 *   Array of facets indexed by facet id.
 */
function _faceted_search_list($module) {
  $function = $module .'_faceted_search_list';
  $facets = $function();

  if (!isset($facets)) {
    return array();
  }
  elseif (!is_array($facets)) {
    return array($facets->get_id() => $facets);
  }
  else {
    $key_facets = array();
    foreach ($facets as $facet) {
      $key_facets[$facet->get_id()] = $facet;
    }
    return $key_facets;
  }
}

/**
 * Utility function to sort facets.
 */
function _faceted_search_compare_facets($a, $b) {
  if ($a->get_weight() == $b->get_weight()) {
    if ($a->get_key() == 'keyword' && $b->get_key() == 'keyword' && $a->is_active() && $b->is_active()) {
      // Special case for sorting keyword facets
      // (Using intermediate variables to please PHP4)
      $a_cat = $a->get_active_category();
      $b_cat = $b->get_active_category();
      return strcmp($a_cat->get_label(), $b_cat->get_label());
    }
    return strcmp($a->get_label(), $b->get_label());
  }
  return ($a->get_weight() < $b->get_weight()) ? -1 : 1;
}
