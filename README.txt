$Id$

README file for the Faceted Search Drupal module.


Description
***********

The Faceted Search module provides a search API and a search interface for
allowing users to navigate content in such a way that they can rapidly get
acquainted with the scope and nature of the content, and never feel lost in the
data. More than a search interface, this is an information navigation and
discovery tool.

The interface exposes metadata in such a way that users can build their queries
as they go, refining or expanding the current query, with results automatically
reflecting the current query. This interface also combines free-text search,
fully leveraging Drupal's search engine. It avoids complex search forms, and
never offers facets that would lead to empty result sets.

The most obvious metadata for faceted searches is provided by Drupal's taxonomy
module. However, Faceted Search's API allows developers to expose other
metadata, therefore providing more more facets to users for browsing content.

Any of the following cases might prompt you to use Faceted Search:

- Users need to filter content using multiple taxonomy terms at the same time.

- Users want to combine text searches, taxonomy term filtering, and other search
  criteria.

- Users don't know precisely what they can find on your site, or what to search
  for.

- You want to hint users at related content they might not have thought of
  looking for, but that could be of interest to them.

- You want to clearly show users what subject areas are the most comprehensive
  on your site.

- You are trying to discover relationships or trends between contents.

- Your site has too much content for it to be displayed through fixed
  navigational structures, but you still want it to be navigable.

- You want to use a faceted classification
  [http://en.wikipedia.org/wiki/Faceted_classification] because a single
  taxonomic order or a single folksonomy is not suitable or sufficient for your
  content.

- Users often get empty result sets when searching your site.

- You think that "advanced" search forms are not fun to use.


The package
***********

Faceted Search is in fact a bundle of modules.

- Faceted Search: Provides the search framework and API.

- Faceted Search UI: Provides the search user interface.

- Faceted Search Views: Allows to use Views to display the search results.

- Author Facet: Allows users to refine the current search based on content
  author.

- Content Type Facet: Allows users to refine the current search based on content
  type.

- Date Authored Facet: Allows users to refine the current search based on
  content creation date.

- Taxonomy Facets: Allows users to search content through taxonomy. Any
  vocabulary can become a facet that can be used to refine the current search.

- Date Facets Format: Provides formatting options for date-based facets.

Hopefully, many more facets will be developed. The API is meant to make it easy
to implement new facets.


Current status
**************

This module is undergoing development. The provided features and API might still
change without warning.

Although this code uses some object-oriented programming, it is still compatible
with PHP 4 at the moment.


Caution
*******

Faceted Search is database-intensive. If your server can barely keep up with
your traffic, this package will make things worst. Make sure to benchmark
performance before deploying this system on a busy site.


Requirements
************

- Drupal 5.x (http://drupal.org/project/drupal).

- MySQL 4.1 (or later version). 


Recommended modules
*******************

- Biblio Facets (http://drupal.org/project/biblio_facets)
  Exposes Biblio (http://drupal.org/project/biblio) types and fields as facets.

- CCK Facets (http://drupal.org/project/cck_facets)
  Exposes Content Construction Kit (CCK) (http://drupal.org/project/cck) fields
  as facets.

- Views (http://drupal.org/project/views): In combination with the Faceted
  Search Views module, the Views module can give you tremendous flexibility for
  displaying Faceted Search's results, and even for performing additional
  filtering of the search results. See the "Views integration" topic below for
  more details.

- jQuery Update (http://drupal.org/project/jquery_update): If you wish to use
  Faceted Search UI's tooltips feature (for showing subcategories when hovering
  over a category in the guided search), it is strongly recommended to install
  the jQuery Update module. Make sure to read that module's installation
  instructions. If you don't use the tooltips feature, Faceted Search UI won't
  use jQuery at all, so in that case you would not need jQuery Update.

- Taxonomy hide (http://drupal.org/project/taxonomy_hide): If you use Faceted
  Search UI's Related Categories block, you might want to remove Drupal's
  default terms listing when viewing a node. You could do that from your site's
  theme, but another way could be to use the Taxonomy hide module.


Known incompatibilities
***********************

- PostgreSQL: PostgreSQL has not actually been tested at this point. Also, the
  Date Authored Facet module uses some MySQL-specific functions.  Feel free to
  share patches to support PostgreSQL (or any other database). :-)
  See http://drupal.org/node/230471 for updates, and remember that you can help
  make PostgreSQL support happen!


Installation
************

1. Extract the 'faceted_search' module directory, including all its
   subdirectories, into your Drupal modules directory.

2. Go to the Administer > Site building > Modules page, and enable the following
   modules:

   - Faceted Search
   - Faceted Search UI
   - At least one of the following modules (which are provided with Faceted Search):
     - Author Facet
     - Content Type Facet
     - Date Authored Facet
     - Taxonomy Facets
   - Search (Drupal core module)
   - Taxonomy (Drupal core module -- only needed if you intend to use Taxonomy Facets)

3. Go to the Administer > Site configuration > Faceted Search page, and click
   the Add Environment tab.

4. Define a faceted search environment by filling the "Add a faceted search
   environment" form. Hopefully it is self-explanatory enough, but don't be
   afraid to experiment. You can always change any of the settings later.

   Click Save to save the new environment. This takes you back to the 
   Administer > Site configuration > Faceted Search page.

5. Go to the Administer > Site building > Blocks page, and enable the following
   blocks (where my_search is the name of the faceted search environment you
   have just created):

   - my_search / Current search
   - my_search / Keyword search
   - my_search / Guided search
   - my_search / Related categories
   - my_search / Sort options

   Use Weight to order the blocks. Having the Current search block located above
   the Keyword search and Guided search blocks is generally most intuitive for
   users.

   When using multiple faceted search environments, you'll want to configure the
   block visibility to avoid showing multiple Keyword search or Guided search
   blocks at the same time. The most common setting is to have those blocks
   visible only on your search environment's base path.

   To do so, in Administer > Site building > Blocks, click Configure next to the
   Keyword search or Guided search block whose visibility is to be adjusted.
   Select "Show on every page except the listed pages", then enter the following
   paths in the Pages field:

   base_path
   base_path/*

   ... where "base_path" should be replaced with your search environment's
   actual base path.

6. Go to the Administer > User management > Access control page, and grant the
   "use faceted search" permission to the roles you intend to give access to
   faceted search.


Views integration
*****************

You may use the Views module to display search results through the Faceted
Search Views module, which allows to display results in an embedded view.

Results shown through a view might differ from those obtained from other display
styles, because the view might provide additional filters. A view must use the
"Faceted Search: Environment ID" argument to become available to Faceted Search
as a display style.

Note also that, because it is embedded in Faceted Search's results page, the
view cannot use exposed filters or URL-based arguments.

Assuming that both Views and Faceted Search are already installed on your site,
here are the step-by-step instructions to use a View to display search results:

1. Go to Administer > Site building > Views, and click Add to create a new view.

2. Enter a name for the view.

3. Choose to provide a Page view. Note that the "Title", "Use pager", "Empty
   text" and "Nodes per Page" options are ignored when the view is embedded
   within Faceted Search's results page.

4. In the Arguments section, add the "Faceted Search: Environment ID" filter.
   You may then select one of Faceted Search's sort options in the argument's
   "Option" field.

5. Save your new view.

6. Go to Administer > Site configuration > Faceted search, choose to edit your
   faceted search environment, and select your new view in the "Display style"
   field of the "Results page" section.


Support
*******

For support requests, bug reports, and feature requests, please use Faceted
Search's issue queue on http://drupal.org/project/issues/faceted_search.

Please DO NOT send bug reports through e-mail or personal contact forms, use the
aforementioned issue queue instead.

For general discussions about Faceted Search (or other Drupal search solutions),
you are invited to join the Search group on http://groups.drupal.org/node/4102.

You may also contact the author for paid customizations to this module
(http://davidlesieur.com/contact).


Credits
*******

- Project initiated and developed by David Lesieur (http://davidlesieur.com,
  http://drupal.org/user/17157).

- Sponsored in part by Laboratoire NT2 (http://www.labo-nt2.uqam.ca) and Eyos BV
  (http://www.eyos.nl).

- The superb Flamenco search interface (http://flamenco.berkeley.edu) has
  provided much inspiration for this project.
