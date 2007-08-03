README file for the Faceted Search Drupal module.


Description
***********

The Faceted Search module provides a search API and a search interface for
allowing users to navigate content in such a way that they can rapidly get
acquainted with the scope and nature of the content, and never feel lost in the
data.

The interface exposes metadata in such a way that users can build their queries
as they go, refining or expanding the current query, with results automatically
reflecting the current query. This interface also integrates free-text search,
fully leveraging Drupal's search engine. It avoids complex search forms (which
break the navigation flow), and never offers facets that would lead to empty
result sets.

The most obvious metadata for faceted searches is provided by Drupal's taxonomy
module. However, Faceted Search's API allows developers to expose other
metadata, therefore providing more more facets to users for browsing content.


The package
***********

Faceted Search is in fact a bundle of modules.

- Faceted Search: This module provides the search framework and API.

- Faceted Search UI: This module provides the search user interface.

- Taxonomy Facets: This modules allows users to search content through
  taxonomy. Any vocabulary can become a facet that can be used to refine the
  current query.

- Content Type Facet: This module adds a facet that allows users to refine the
  current query based on content type.

- Author Facet: This module adds a facet that allows users to refine the current
  query based on content author.

Hopefully, many more facets will be developed. The API is meant to make it easy
to implement new facets.


Current status
**************

This module is undergoing development. It might not be a good idea to use it on
a production site.

The interface still has some rough edges, but should work pretty well.

Although this code uses some object-oriented programming, it is still compatible
with PHP 4 at the moment.


Requirements
************

- This module requires Drupal 5.x (http://drupal.org/project/drupal).

- If you wish to use Faceted Search UI's tooltips feature (for showing
  subcategories when hovering over a category in the guided search), it is
  strongly recommended to install the jQuery Update module
  (http://drupal.org/project/jquery_update). Make sure to read that module's
  installation instructions. If you don't use the tooltips feature, Faceted
  Search UI won't use jQuery at all, so in that case you would not need jQuery
  Update.

- If you use the Faceted Search UI's Related Categories block, you might want to
  remove Drupal's default terms listing when viewing a node. To do that, see the
  Taxonomy hide module (http://drupal.org/project/taxonomy_hide).


Caution
*******

Faceted searches are database-intensive. If your server can barely keep up with
your traffic, this package will make things worst. Make sure to benchmark
performance before deploying this package on a busy site.


Installation
************

1. Extract the 'faceted_search' module directory, including all its
   subdirectories, into your Drupal modules directory.

2. Go to the Administer > Site building > Modules page, and enable the following
   modules:

   - Faceted Search
   - Faceted Search UI
   - Taxonomy Facets and/or Content Type Facet and/or Author Facet
   - Search (Drupal core module)
   - Taxonomy (Drupal core module -- only needed if you intend to use Taxonomy Facets)

3. Go to the Administer > User management > Access control page, and grant the
   "use faceted search" permission to the roles you intend to give access to
   faceted search.

4. Go to the Administer > Site building > Blocks page, and enable the following
   blocks:

   - Faceted search / Current search
   - Faceted search / Keyword search
   - Faceted search / Guided search
   - Faceted search / Related categories

   It is most intuitive for users to have the Current search block located above
   the Keyword search and Guided search blocks, and to have the Related
   categories block placed in the Content region (so it appears under nodes).

6. Go to the Administer > Site configuration > Faceted Search page.

6a. If you have enabled the Taxonomy Facets module, click the Taxonomy Facets
    tab and check the vocabularies you'd like to use in faceted searches.

    If your site does not have vocabularies, you'll have to create them, and
    populate them with terms. See the Drupal handbook pages on taxonomy for more
    information (http://drupal.org/handbook/modules/taxonomy).

6b. If you have enabled the Content Type Facet module, click the Content Type
    Facet tab and check the content types you'd like to use in faceted searches.

6c. If you have enabled the Author Facet module, click the Author Facet tab and
    check the roles you'd like to exclude from faceted searches.

7. Click the Settings tab and assign weights and maximum number of categories to
   each facet. These options apply to the Current search and Guided search
   blocks.


Credits
*******

* Developed by David Lesieur (http://davidlesieur.com,
  http://drupal.org/user/17157).

* The superb Flamenco search interface (http://flamenco.berkeley.edu) provided,
  and still provides, most of the inspiration for this project.

