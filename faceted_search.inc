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
   * Constructor.
   */
  function faceted_search_category() {
  }
  
  /**
   * Return the subcategories, if any.
   *
   * This only needs to be overridden for hierarchical facets.
   */
  function get_subcategories() {
    return array();
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
   * The maximum number of categories to show in this facet.
   */
  var $_max_categories = 0;

  /**
   * The active category of this facet.
   */
  var $_active = NULL;
  
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
   * @param $active_category
   *   The category to set as this facet's active category. Give NULL for an
   *   unused facet. Unused facets normally do not appear in the UI's view of
   *   the current search, but can be available for browsing.
   *
   * @param $active_path
   *   Array representing the path of ancestor categories leading to the
   *   specified active category. These must be actual parents of the active
   *   category. Give an empty array if the active category is a root category
   *   of the facet, or if there is no active category.
   */
  function faceted_search_facet($key, $active_category = NULL, $active_path = array()) {
    $this->_key = $key;
    $this->_path = $active_path;
    $this->_active = $active_category;
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
   * Return the active category of this facet, or NULL if this facet has no
   * active category.
   */
  function get_active_category() {
    return $this->_active;
  }

  /**
   * Set the active category of this facet. The caller must ensure consistency
   * with the active path.
   */
  function set_active_category($category) {
    $this->_active = $category;
  }
  
  /**
   * Return the path of ancestors to the active category (without the active
   * category itself), or an empty array if the active category has no parent or
   * if there is no active category.
   */
  function get_active_path() {
    return $this->_path;
  }

  /**
   * Set the path of ancestors leading to the active category. The caller must
   * ensure consistency with the active category.
   */
  function set_active_path($path) {
    $this->_path = $path;
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
    parent::faceted_search_facet('keyword', $active_category);
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
    return -20;
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
    return -19;
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
    return -18;
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
    return -17;
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
   * Perform the search and store the results in a temporary table.
   *
   * Two queries are performed. The first query selects a set of possible
   * matches based on the search index and the given keywords. This is the
   * classic "OR" search.
   *
   * The second query further refines this set by verifying advanced conditions
   * (such as facets, AND, negative or phrase matches), and orders the results
   * by score.
   */
  function execute() {
    if (!$this->_keywords && !$this->_facets) {
      return array(); // Nothing to search
    }
    
    // Build first query
    
    $query1 = new _faceted_search_query();
    list($rewrite_join, $rewrite_where) = _db_rewrite_sql();
    if ($rewrite_join) {
      $query1->joins[] = $rewrite_join;
    }
    if ($rewrite_where) {
      $query1->wheres[] = $rewrite_where;
    }
    $query1->wheres[] = 'n.status = 1';

    $query_parts = search_parse_query($this->_keywords);
    if ($query_parts[2] && $query_parts[3]) {
      $query1->wheres[] = $query_parts[2];
      $query1->arguments = array_merge($query1->arguments, $query_parts[3]);
    }
    $query1->having[] = 'COUNT(*) >= %d';
    $query1->arguments[] = $query_parts[4];

    if ($query_parts[0] || $query_parts[2]) {
      // Has some positive or negative keywords
      $query1->select = 'i.sid AS nid, SUM(i.score * t.count) AS relevance, COUNT(*) AS matches';
      $query1->from = '{search_index} i';
      $query1->wheres[] = "i.type = 'node'";
      $query1->groupby[] = 'i.sid';
      array_unshift($query1->joins, 'INNER JOIN {node} n ON n.nid = i.sid INNER JOIN {users} u ON n.uid = u.uid INNER JOIN {search_total} t ON i.word = t.word');
    }
    else {
      // Has no keywords
      $query1->select = 'n.nid, 1 AS relevance, COUNT(*) AS matches'; // TODO: check if relevance = 1 makes sense
      $query1->from = '{node} n';
      $query1->joins[] = 'INNER JOIN {users} u ON n.uid = u.uid';
      $query1->groupby[] = 'n.nid';
    }
  
    // Build second query
    
    $query2 = new _faceted_search_query();
    // Based on node_search() - BEGIN
    // Build ranking expression (we try to map each parameter to a
    // uniform distribution in the range 0..1).
    $ranking = array();
    if ($query_parts[0] || $query_parts[2]) {
      // Has some positive or negative keywords
      $query2->joins[] = 'INNER JOIN {search_dataset} d ON i.nid = d.sid';
    }
    $query2->joins[] = 'INNER JOIN {node} n ON n.nid = i.nid'; // Always join the node table
    // Used to avoid joining on node_comment_statistics twice
    $stats_join = FALSE;
    $this->_results_total = 0;
    if ($weight = (int)variable_get('node_rank_relevance', 5)) {
      // Average relevance values hover around 0.15
      $ranking[] = '%d * i.relevance';
      $query2->arguments[] = $weight;
      $this->_results_total += $weight;
    }
    if ($weight = (int)variable_get('node_rank_recent', 5)) {
      // Exponential decay with half-life of 6 months, starting at last indexed node
      $ranking[] = '%d * POW(2, (GREATEST(n.created, n.changed, IFNULL(c.last_comment_timestamp, 0)) - %d) * 6.43e-8)'; // TODO: see http://drupal.org/node/106659
      $query2->arguments[] = $weight;
      $query2->arguments[] = (int)variable_get('node_cron_last', 0);
      $query2->joins[] = 'LEFT JOIN {node_comment_statistics} c ON c.nid = i.nid';
      $stats_join = TRUE;
      $this->_results_total += $weight;
    }
    if (module_exists('comment') && $weight = (int)variable_get('node_rank_comments', 5)) {
      // Inverse law that maps the highest reply count on the site to 1 and 0 to 0.
      $scale = variable_get('node_cron_comments_scale', 0.0);
      $ranking[] = '%d * (2.0 - 2.0 / (1.0 + IFNULL(c.comment_count, 0) * %f))';
      $query2->arguments[] = $weight;
      $query2->arguments[] = $scale;
      if (!$stats_join) {
        $query2->joins[] = 'LEFT JOIN {node_comment_statistics} c ON c.nid = i.nid';
      }
      $this->_results_total += $weight;
    }
    if (module_exists('statistics') && variable_get('statistics_count_content_views', 0) && $weight = (int)variable_get('node_rank_views', 5)) {
      // Inverse law that maps the highest view count on the site to 1 and 0 to 0.
      $scale = variable_get('node_cron_views_scale', 0.0);
      $ranking[] = '%d * (2.0 - 2.0 / (1.0 + IFNULL(nc.totalcount, 0) * %f))';
      $query2->arguments[] = $weight;
      $query2->arguments[] = $scale;
      $query2->joins[] = 'LEFT JOIN {node_counter} nc ON nc.nid = i.nid';
      $this->_results_total += $weight;
    }
    $query2->select = 'i.nid, ('. (count($ranking) ? implode(' + ', $ranking) : 'i.relevance') .') / '. $this->_results_total .' AS score';
    // Based on node_search() - END
    if ($query_parts[0] && $query_parts[1]) {
      $query2->wheres[] = $query_parts[0];
      $query2->arguments = array_merge($query2->arguments, $query_parts[1]);
    }
    $query2->orderby = 'score DESC';

    // Apply facets into query, except for keywords (which are already included
    // in the query)
    foreach ($this->_facets as $facet) {
      if ($facet->get_key() != 'keyword') {
        $active = $facet->get_active_category();
        if ($active) {
          $query2->build($active->get_results_query());
        }
      }
    }

    // Perform search
    $this->_query($query1, $query2);
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
   * Count nodes matching the given category within results. If this search has
   * no results, the count is done within all nodes.
   */
  function count_within_results($category) {
    $query = new _faceted_search_query();
    if ($this->_results_count > 0) {
      $query->select = 'COUNT(*)';
      $query->from = $this->_results_table .' results';
    }
    elseif (empty($this->_text)) {
      // No search text, search within all nodes
      $query->select = 'COUNT(*)';
      $query->from = '{node} results';
      $query->wheres[] = 'results.status = 1';
      // TODO: this needs cleanup. Different tables are used depending on
      // context and use the same alias... Not so intuitive.
    }
    else {
      // There is a search text but the results count is 0, thus no search
      // refinement si possible!
      return 0; 
    }
    $query->build($category->get_count_within_results_query());
    return db_result(db_query($query->sql(), $query->arguments));
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
    // Retrieve the settings for this facet
    $settings = variable_get('faceted_search_display', array());
    $key = $module .'_'. $facet->get_id();
    
    // Assign this facet's settings
    $facet->_weight = isset($settings[$key]['weight']) ? $settings[$key]['weight'] : $facet->get_default_weight();
    $facet->_max_categories = isset($settings[$key]['max']) ? $settings[$key]['max'] : $facet->get_default_max_categories();

    // Add facet to this search
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
   *
   * @return
   *   An array of nids for the search results.
   */
  function _query($query1, $query2) {
    // This is similar to search.module's do_search()

    // Drop query1's temporary table if it exists -- this allows for multiple
    // faceted searches per page request
    db_query('DROP TEMPORARY TABLE IF EXISTS temp_faceted_search_nids'); 
    
    // Execute query1
    $result = db_query_temporary($query1->sql(), $query1->arguments, 'temp_faceted_search_nids');

    // Calculate maximum relevance, to normalize it
    $normalize = db_result(db_query('SELECT MAX(relevance) FROM temp_faceted_search_nids'));
    if (!$normalize) {
      return array();
    }

    // Execute query2
    $query2->select = str_replace('i.relevance', '('. (1.0 / $normalize) .' * i.relevance)', $query2->select);
    $query2->from = 'temp_faceted_search_nids i'; // Query from the temporary table generated by query1
    $result = db_query_temporary($query2->sql(), $query2->arguments, $this->_results_table);
    
    $this->_results_count = db_result(db_query('SELECT COUNT(*) FROM '. $this->_results_table));
    if ($this->_results_count == 0) {
      return array();
    }
  }
}

/**
 * Utility class to store and build search queries.
 */
class _faceted_search_query {
  var $select = '';
  var $from = '';
  var $joins = array();
  var $wheres = array();
  var $arguments = array();
  var $groupby = array();
  var $having = array();
  var $orderby = '';

  /**
   * Insert the specified parts into the query.
   *
   * @param $parts
   *   A structured array with the following items:
   *
   *   'joins' => Array of JOIN clauses.
   *
   *   'wheres' => Array of WHERE clauses.
   *
   *   'arguments' => Array of values to later pass as argument to
   *   db_query(). Use this whenever possible (instead of putting values
   *   directly in 'wheres', to make sure that arguments are escaped properly.
   *
   *   'groupby' => Array of GROUP BY clauses.
   *
   *   'having' => Array of HAVING clauses.
   */
  function build($parts) {
    // TODO: Have a limit on the maximum number of joins
    // TODO: Have a threshold for join operation (at some point, STRAIGHT_JOIN
    // becomes faster than INNER JOIN. Probably requires to split join op, join
    // table and join cond
    foreach ($parts as $key => $array) {
      $this->$key = array_merge($this->$key, $array);
    }
  }

  /**
   * Build the actual SQL query text.
   */
  function sql() {
    return 'SELECT '. $this->select .' FROM '. $this->from .' '. implode(' ', $this->joins) . (count($this->wheres) ? ' WHERE '. implode(' AND ', $this->wheres) : '') . (count($this->groupby) ? ' GROUP BY '. implode(', ', $this->groupby) : '') . (count($this->having) ? ' HAVING '. implode(', ', $this->having) : '') . ($this->orderby ? ' ORDER BY '. $this->orderby : '');
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
    if ($a->get_key() == 'keyword' && $b->get_key() == 'keyword' && $a->get_active_category() && $b->get_active_category()) {
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
