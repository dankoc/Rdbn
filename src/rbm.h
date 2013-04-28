  


<!DOCTYPE html>
<html>
  <head prefix="og: http://ogp.me/ns# fb: http://ogp.me/ns/fb# githubog: http://ogp.me/ns/fb/githubog#">
    <meta charset='utf-8'>
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
        <title>Rdbn/src/rbm.h at 188e756a9c91eca30b4d0e37c20b39aafc2d94a4 · dankoc/Rdbn</title>
    <link rel="search" type="application/opensearchdescription+xml" href="/opensearch.xml" title="GitHub" />
    <link rel="fluid-icon" href="https://github.com/fluidicon.png" title="GitHub" />
    <link rel="apple-touch-icon" sizes="57x57" href="/apple-touch-icon-114.png" />
    <link rel="apple-touch-icon" sizes="114x114" href="/apple-touch-icon-114.png" />
    <link rel="apple-touch-icon" sizes="72x72" href="/apple-touch-icon-144.png" />
    <link rel="apple-touch-icon" sizes="144x144" href="/apple-touch-icon-144.png" />
    <link rel="logo" type="image/svg" href="http://github-media-downloads.s3.amazonaws.com/github-logo.svg" />
    <link rel="xhr-socket" href="/_sockets">
    <meta name="msapplication-TileImage" content="/windows-tile.png">
    <meta name="msapplication-TileColor" content="#ffffff">

    
    
    <link rel="icon" type="image/x-icon" href="/favicon.ico" />

    <meta content="authenticity_token" name="csrf-param" />
<meta content="7rceYxFgM6P2L2pNvNKFpIKiTFT9LFNNgMhmg86R71U=" name="csrf-token" />

    <link href="https://a248.e.akamai.net/assets.github.com/assets/github-21d1f919c9b16786238504ad1232b4937bbdd088.css" media="all" rel="stylesheet" type="text/css" />
    <link href="https://a248.e.akamai.net/assets.github.com/assets/github2-2be8c95d7426dd9f223af4019da16db22547f7a8.css" media="all" rel="stylesheet" type="text/css" />
    


      <script src="https://a248.e.akamai.net/assets.github.com/assets/frameworks-92d138f450f2960501e28397a2f63b0f100590f0.js" type="text/javascript"></script>
      <script src="https://a248.e.akamai.net/assets.github.com/assets/github-8b597885ade82d6a3d9108d93ea8e86b9f294d91.js" type="text/javascript"></script>
      
      <meta http-equiv="x-pjax-version" content="77a530946004f4df817ad3a299a913ed">

        <link data-pjax-transient rel='permalink' href='/dankoc/Rdbn/blob/188e756a9c91eca30b4d0e37c20b39aafc2d94a4/src/rbm.h'>
    <meta property="og:title" content="Rdbn"/>
    <meta property="og:type" content="githubog:gitrepository"/>
    <meta property="og:url" content="https://github.com/dankoc/Rdbn"/>
    <meta property="og:image" content="https://secure.gravatar.com/avatar/4d95a2aa7ca980af24d543997aa32add?s=420&amp;d=https://a248.e.akamai.net/assets.github.com%2Fimages%2Fgravatars%2Fgravatar-user-420.png"/>
    <meta property="og:site_name" content="GitHub"/>
    <meta property="og:description" content="Rdbn - Fast code to learn deep belief networks in R."/>
    <meta property="twitter:card" content="summary"/>
    <meta property="twitter:site" content="@GitHub">
    <meta property="twitter:title" content="dankoc/Rdbn"/>

    <meta name="description" content="Rdbn - Fast code to learn deep belief networks in R." />

  <link href="https://github.com/dankoc/Rdbn/commits/188e756a9c91eca30b4d0e37c20b39aafc2d94a4.atom" rel="alternate" title="Recent Commits to Rdbn:188e756a9c91eca30b4d0e37c20b39aafc2d94a4" type="application/atom+xml" />

  </head>


  <body class="logged_in page-blob windows vis-public env-production  ">
    <div id="wrapper">

      

      
      
      

      <div class="header header-logged-in true">
  <div class="container clearfix">

    <a class="header-logo-invertocat" href="https://github.com/">
  <span class="mega-icon mega-icon-invertocat"></span>
</a>

    <div class="divider-vertical"></div>

      <a href="/notifications" class="notification-indicator tooltipped downwards" title="You have no unread notifications">
    <span class="mail-status all-read"></span>
  </a>
  <div class="divider-vertical"></div>


      <div class="command-bar js-command-bar  ">
            <form accept-charset="UTF-8" action="/search" class="command-bar-form" id="top_search_form" method="get">
  <a href="/search/advanced" class="advanced-search-icon tooltipped downwards command-bar-search" id="advanced_search" title="Advanced search"><span class="mini-icon mini-icon-advanced-search "></span></a>

  <input type="text" data-hotkey="/ s" name="q" id="js-command-bar-field" placeholder="Search or type a command" tabindex="1" data-username="dankoc" autocapitalize="off">

  <span class="mini-icon help tooltipped downwards" title="Show command bar help">
    <span class="mini-icon mini-icon-help"></span>
  </span>

  <input type="hidden" name="ref" value="cmdform">

    <input type="hidden" class="js-repository-name-with-owner" value="dankoc/Rdbn"/>
    <input type="hidden" class="js-repository-branch" value=""/>
    <input type="hidden" class="js-repository-tree-sha" value="f0242484ff4450a08ba19aaf9b5af08c785921ef"/>

  <div class="divider-vertical"></div>
</form>
        <ul class="top-nav">
            <li class="explore"><a href="https://github.com/explore">Explore</a></li>
            <li><a href="https://gist.github.com">Gist</a></li>
            <li><a href="/blog">Blog</a></li>
          <li><a href="http://help.github.com">Help</a></li>
        </ul>
      </div>

    

  

    <ul id="user-links">
      <li>
        <a href="https://github.com/dankoc" class="name">
          <img height="20" src="https://secure.gravatar.com/avatar/4d95a2aa7ca980af24d543997aa32add?s=140&amp;d=https://a248.e.akamai.net/assets.github.com%2Fimages%2Fgravatars%2Fgravatar-user-420.png" width="20" /> dankoc
        </a>
      </li>

        <li>
          <a href="/new" id="new_repo" class="tooltipped downwards" title="Create a new repo">
            <span class="mini-icon mini-icon-create"></span>
          </a>
        </li>

        <li>
          <a href="/settings/profile" id="account_settings"
            class="tooltipped downwards"
            title="Account settings ">
            <span class="mini-icon mini-icon-account-settings"></span>
          </a>
        </li>
        <li>
          <a class="tooltipped downwards" href="/logout" data-method="post" id="logout" title="Sign out">
            <span class="mini-icon mini-icon-logout"></span>
          </a>
        </li>

    </ul>


<div class="js-new-dropdown-contents hidden">
  <ul class="dropdown-menu">
    <li>
      <a href="/new"><span class="mini-icon mini-icon-create"></span> New repository</a>
    </li>
    <li>
        <a href="https://github.com/dankoc/Rdbn/issues/new"><span class="mini-icon mini-icon-issue-opened"></span> New issue</a>
    </li>
    <li>
    </li>
    <li>
      <a href="/organizations/new"><span class="mini-icon mini-icon-u-list"></span> New organization</a>
    </li>
  </ul>
</div>


    
  </div>
</div>

      

      

      


            <div class="site hfeed" itemscope itemtype="http://schema.org/WebPage">
      <div class="hentry">
        
        <div class="pagehead repohead instapaper_ignore readability-menu ">
          <div class="container">
            <div class="title-actions-bar">
              

<ul class="pagehead-actions">

    <li class="nspr">
      <a href="/dankoc/Rdbn/pull/new/188e756a9c91eca30b4d0e37c20b39aafc2d94a4" class="button minibutton btn-pull-request" icon_class="mini-icon-pull-request"><span class="mini-icon mini-icon-pull-request"></span>Pull Request</a>
    </li>

    <li class="subscription">
      <form accept-charset="UTF-8" action="/notifications/subscribe" data-autosubmit="true" data-remote="true" method="post"><div style="margin:0;padding:0;display:inline"><input name="authenticity_token" type="hidden" value="7rceYxFgM6P2L2pNvNKFpIKiTFT9LFNNgMhmg86R71U=" /></div>  <input id="repository_id" name="repository_id" type="hidden" value="8807042" />

    <div class="select-menu js-menu-container js-select-menu">
      <span class="minibutton select-menu-button js-menu-target">
        <span class="js-select-button">
          <span class="mini-icon mini-icon-unwatch"></span>
          Unwatch
        </span>
      </span>

      <div class="select-menu-modal-holder js-menu-content">
        <div class="select-menu-modal">
          <div class="select-menu-header">
            <span class="select-menu-title">Notification status</span>
            <span class="mini-icon mini-icon-remove-close js-menu-close"></span>
          </div> <!-- /.select-menu-header -->

          <div class="select-menu-list js-navigation-container">

            <div class="select-menu-item js-navigation-item ">
              <span class="select-menu-item-icon mini-icon mini-icon-confirm"></span>
              <div class="select-menu-item-text">
                <input id="do_included" name="do" type="radio" value="included" />
                <h4>Not watching</h4>
                <span class="description">You only receive notifications for discussions in which you participate or are @mentioned.</span>
                <span class="js-select-button-text hidden-select-button-text">
                  <span class="mini-icon mini-icon-watching"></span>
                  Watch
                </span>
              </div>
            </div> <!-- /.select-menu-item -->

            <div class="select-menu-item js-navigation-item selected">
              <span class="select-menu-item-icon mini-icon mini-icon-confirm"></span>
              <div class="select-menu-item-text">
                <input checked="checked" id="do_subscribed" name="do" type="radio" value="subscribed" />
                <h4>Watching</h4>
                <span class="description">You receive notifications for all discussions in this repository.</span>
                <span class="js-select-button-text hidden-select-button-text">
                  <span class="mini-icon mini-icon-unwatch"></span>
                  Unwatch
                </span>
              </div>
            </div> <!-- /.select-menu-item -->

            <div class="select-menu-item js-navigation-item ">
              <span class="select-menu-item-icon mini-icon mini-icon-confirm"></span>
              <div class="select-menu-item-text">
                <input id="do_ignore" name="do" type="radio" value="ignore" />
                <h4>Ignoring</h4>
                <span class="description">You do not receive any notifications for discussions in this repository.</span>
                <span class="js-select-button-text hidden-select-button-text">
                  <span class="mini-icon mini-icon-mute"></span>
                  Stop ignoring
                </span>
              </div>
            </div> <!-- /.select-menu-item -->

          </div> <!-- /.select-menu-list -->

        </div> <!-- /.select-menu-modal -->
      </div> <!-- /.select-menu-modal-holder -->
    </div> <!-- /.select-menu -->

</form>
    </li>

    <li class="js-toggler-container js-social-container starring-container ">
      <a href="/dankoc/Rdbn/unstar" class="minibutton js-toggler-target star-button starred upwards" title="Unstar this repo" data-remote="true" data-method="post" rel="nofollow">
        <span class="mini-icon mini-icon-remove-star"></span>
        <span class="text">Unstar</span>
      </a>
      <a href="/dankoc/Rdbn/star" class="minibutton js-toggler-target star-button unstarred upwards" title="Star this repo" data-remote="true" data-method="post" rel="nofollow">
        <span class="mini-icon mini-icon-star"></span>
        <span class="text">Star</span>
      </a>
      <a class="social-count js-social-count" href="/dankoc/Rdbn/stargazers">0</a>
    </li>

        <li>
          <a href="/dankoc/Rdbn/fork" class="minibutton js-toggler-target fork-button lighter upwards" title="Fork this repo" rel="nofollow" data-method="post">
            <span class="mini-icon mini-icon-branch-create"></span>
            <span class="text">Fork</span>
          </a>
          <a href="/dankoc/Rdbn/network" class="social-count">0</a>
        </li>


</ul>

              <h1 itemscope itemtype="http://data-vocabulary.org/Breadcrumb" class="entry-title public">
                <span class="repo-label"><span>public</span></span>
                <span class="mega-icon mega-icon-public-repo"></span>
                <span class="author vcard">
                  <a href="/dankoc" class="url fn" itemprop="url" rel="author">
                  <span itemprop="title">dankoc</span>
                  </a></span> /
                <strong><a href="/dankoc/Rdbn" class="js-current-repository">Rdbn</a></strong>
              </h1>
            </div>

            
  <ul class="tabs">
    <li class="pulse-nav"><a href="/dankoc/Rdbn/pulse" highlight="pulse" rel="nofollow"><span class="mini-icon mini-icon-pulse"></span></a></li>
    <li><a href="/dankoc/Rdbn" class="selected" highlight="repo_source repo_downloads repo_commits repo_tags repo_branches">Code</a></li>
    <li><a href="/dankoc/Rdbn/network" highlight="repo_network">Network</a></li>
    <li><a href="/dankoc/Rdbn/pulls" highlight="repo_pulls">Pull Requests <span class='counter'>0</span></a></li>

      <li><a href="/dankoc/Rdbn/issues" highlight="repo_issues">Issues <span class='counter'>0</span></a></li>

      <li><a href="/dankoc/Rdbn/wiki" highlight="repo_wiki">Wiki</a></li>


    <li><a href="/dankoc/Rdbn/graphs" highlight="repo_graphs repo_contributors">Graphs</a></li>

      <li>
        <a href="/dankoc/Rdbn/settings">Settings</a>
      </li>

  </ul>
  
<div class="tabnav">

  <span class="tabnav-right">
    <ul class="tabnav-tabs">
          <li><a href="/dankoc/Rdbn/tags" class="tabnav-tab" highlight="repo_tags">Tags <span class="counter blank">0</span></a></li>
    </ul>
    
  </span>

  <div class="tabnav-widget scope">


    <div class="select-menu js-menu-container js-select-menu js-branch-menu">
      <a class="minibutton select-menu-button js-menu-target" data-hotkey="w" data-ref="">
        <span class="mini-icon mini-icon-tree"></span>
        <i>tree:</i>
        <span class="js-select-button">188e756a9c</span>
      </a>

      <div class="select-menu-modal-holder js-menu-content js-navigation-container">

        <div class="select-menu-modal">
          <div class="select-menu-header">
            <span class="select-menu-title">Switch branches/tags</span>
            <span class="mini-icon mini-icon-remove-close js-menu-close"></span>
          </div> <!-- /.select-menu-header -->

          <div class="select-menu-filters">
            <div class="select-menu-text-filter">
              <input type="text" id="commitish-filter-field" class="js-filterable-field js-navigation-enable" placeholder="Find or create a branch…">
            </div>
            <div class="select-menu-tabs">
              <ul>
                <li class="select-menu-tab">
                  <a href="#" data-tab-filter="branches" class="js-select-menu-tab">Branches</a>
                </li>
                <li class="select-menu-tab">
                  <a href="#" data-tab-filter="tags" class="js-select-menu-tab">Tags</a>
                </li>
              </ul>
            </div><!-- /.select-menu-tabs -->
          </div><!-- /.select-menu-filters -->

          <div class="select-menu-list select-menu-tab-bucket js-select-menu-tab-bucket css-truncate" data-tab-filter="branches">

            <div data-filterable-for="commitish-filter-field" data-filterable-type="substring">

                <div class="select-menu-item js-navigation-item ">
                  <span class="select-menu-item-icon mini-icon mini-icon-confirm"></span>
                  <a href="/dankoc/Rdbn/blob/master/src/rbm.h" class="js-navigation-open select-menu-item-text js-select-button-text css-truncate-target" data-name="master" rel="nofollow" title="master">master</a>
                </div> <!-- /.select-menu-item -->
            </div>

              <form accept-charset="UTF-8" action="/dankoc/Rdbn/branches" class="js-create-branch select-menu-item select-menu-new-item-form js-navigation-item js-new-item-form" method="post"><div style="margin:0;padding:0;display:inline"><input name="authenticity_token" type="hidden" value="7rceYxFgM6P2L2pNvNKFpIKiTFT9LFNNgMhmg86R71U=" /></div>

                <span class="mini-icon mini-icon-branch-create select-menu-item-icon"></span>
                <div class="select-menu-item-text">
                  <h4>Create branch: <span class="js-new-item-name"></span></h4>
                  <span class="description">from ‘188e756a9c91eca30b4d0e37c20b39aafc2d94a4’</span>
                </div>
                <input type="hidden" name="name" id="name" class="js-new-item-value">
                <input type="hidden" name="branch" id="branch" value="188e756a9c91eca30b4d0e37c20b39aafc2d94a4" />
                <input type="hidden" name="path" id="branch" value="src/rbm.h" />
              </form> <!-- /.select-menu-item -->

          </div> <!-- /.select-menu-list -->


          <div class="select-menu-list select-menu-tab-bucket js-select-menu-tab-bucket css-truncate" data-tab-filter="tags">
            <div data-filterable-for="commitish-filter-field" data-filterable-type="substring">

            </div>

            <div class="select-menu-no-results">Nothing to show</div>

          </div> <!-- /.select-menu-list -->

        </div> <!-- /.select-menu-modal -->
      </div> <!-- /.select-menu-modal-holder -->
    </div> <!-- /.select-menu -->

  </div> <!-- /.scope -->

  <ul class="tabnav-tabs">
    <li><a href="/dankoc/Rdbn" class="selected tabnav-tab" highlight="repo_source">Files</a></li>
    <li><a href="/dankoc/Rdbn/commits/" class="tabnav-tab" highlight="repo_commits">Commits</a></li>
    <li><a href="/dankoc/Rdbn/branches" class="tabnav-tab" highlight="repo_branches" rel="nofollow">Branches <span class="counter ">1</span></a></li>
  </ul>

</div>

  
  
  


            
          </div>
        </div><!-- /.repohead -->

        <div id="js-repo-pjax-container" class="container context-loader-container" data-pjax-container>
          


<!-- blob contrib key: blob_contributors:v21:5b0042318ff3f95b156a0b9499854335 -->
<!-- blob contrib frag key: views10/v8/blob_contributors:v21:5b0042318ff3f95b156a0b9499854335 -->


<div id="slider">
    <div class="frame-meta">

      <p title="This is a placeholder element" class="js-history-link-replace hidden"></p>

        <div class="breadcrumb">
          <span class='bold'><span itemscope="" itemtype="http://data-vocabulary.org/Breadcrumb"><a href="/dankoc/Rdbn/tree/188e756a9c91eca30b4d0e37c20b39aafc2d94a4" class="js-slide-to" data-branch="188e756a9c91eca30b4d0e37c20b39aafc2d94a4" data-direction="back" itemscope="url" rel="nofollow"><span itemprop="title">Rdbn</span></a></span></span><span class="separator"> / </span><span itemscope="" itemtype="http://data-vocabulary.org/Breadcrumb"><a href="/dankoc/Rdbn/tree/188e756a9c91eca30b4d0e37c20b39aafc2d94a4/src" class="js-slide-to" data-branch="188e756a9c91eca30b4d0e37c20b39aafc2d94a4" data-direction="back" itemscope="url" rel="nofollow"><span itemprop="title">src</span></a></span><span class="separator"> / </span><strong class="final-path">rbm.h</strong> <span class="js-zeroclipboard zeroclipboard-button" data-clipboard-text="src/rbm.h" data-copied-hint="copied!" title="copy to clipboard"><span class="mini-icon mini-icon-clipboard"></span></span>
        </div>

      <a href="/dankoc/Rdbn/find/188e756a9c91eca30b4d0e37c20b39aafc2d94a4" class="js-slide-to" data-hotkey="t" style="display:none">Show File Finder</a>


        <div class="commit commit-loader file-history-tease js-deferred-content" data-url="/dankoc/Rdbn/contributors/188e756a9c91eca30b4d0e37c20b39aafc2d94a4/src/rbm.h">
          Fetching contributors…

          <div class="participation">
            <p class="loader-loading"><img alt="Octocat-spinner-32-eaf2f5" height="16" src="https://a248.e.akamai.net/assets.github.com/images/spinners/octocat-spinner-32-EAF2F5.gif?1338956357" width="16" /></p>
            <p class="loader-error">Cannot retrieve contributors at this time</p>
          </div>
        </div>

    </div><!-- ./.frame-meta -->

    <div class="frames">
      <div class="frame" data-permalink-url="/dankoc/Rdbn/blob/188e756a9c91eca30b4d0e37c20b39aafc2d94a4/src/rbm.h" data-title="Rdbn/src/rbm.h at 188e756a9c91eca30b4d0e37c20b39aafc2d94a4 · dankoc/Rdbn · GitHub" data-type="blob">

        <div id="files" class="bubble">
          <div class="file">
            <div class="meta">
              <div class="info">
                <span class="icon"><b class="mini-icon mini-icon-text-file"></b></span>
                <span class="mode" title="File Mode">file</span>
                  <span>95 lines (73 sloc)</span>
                <span>4.076 kb</span>
              </div>
              <div class="actions">
                <div class="button-group">
                      <a class="minibutton js-entice" href=""
                         data-entice="You must be signed in and on a branch to make or propose changes">Edit</a>
                  <a href="/dankoc/Rdbn/raw/188e756a9c91eca30b4d0e37c20b39aafc2d94a4/src/rbm.h" class="button minibutton " id="raw-url">Raw</a>
                    <a href="/dankoc/Rdbn/blame/188e756a9c91eca30b4d0e37c20b39aafc2d94a4/src/rbm.h" class="button minibutton ">Blame</a>
                  <a href="/dankoc/Rdbn/commits/188e756a9c91eca30b4d0e37c20b39aafc2d94a4/src/rbm.h" class="button minibutton " rel="nofollow">History</a>
                </div><!-- /.button-group -->
              </div><!-- /.actions -->

            </div>
                <div class="blob-wrapper data type-c js-blob-data">
      <table class="file-code file-diff">
        <tr class="file-code-line">
          <td class="blob-line-nums">
            <span id="L1" rel="#L1">1</span>
<span id="L2" rel="#L2">2</span>
<span id="L3" rel="#L3">3</span>
<span id="L4" rel="#L4">4</span>
<span id="L5" rel="#L5">5</span>
<span id="L6" rel="#L6">6</span>
<span id="L7" rel="#L7">7</span>
<span id="L8" rel="#L8">8</span>
<span id="L9" rel="#L9">9</span>
<span id="L10" rel="#L10">10</span>
<span id="L11" rel="#L11">11</span>
<span id="L12" rel="#L12">12</span>
<span id="L13" rel="#L13">13</span>
<span id="L14" rel="#L14">14</span>
<span id="L15" rel="#L15">15</span>
<span id="L16" rel="#L16">16</span>
<span id="L17" rel="#L17">17</span>
<span id="L18" rel="#L18">18</span>
<span id="L19" rel="#L19">19</span>
<span id="L20" rel="#L20">20</span>
<span id="L21" rel="#L21">21</span>
<span id="L22" rel="#L22">22</span>
<span id="L23" rel="#L23">23</span>
<span id="L24" rel="#L24">24</span>
<span id="L25" rel="#L25">25</span>
<span id="L26" rel="#L26">26</span>
<span id="L27" rel="#L27">27</span>
<span id="L28" rel="#L28">28</span>
<span id="L29" rel="#L29">29</span>
<span id="L30" rel="#L30">30</span>
<span id="L31" rel="#L31">31</span>
<span id="L32" rel="#L32">32</span>
<span id="L33" rel="#L33">33</span>
<span id="L34" rel="#L34">34</span>
<span id="L35" rel="#L35">35</span>
<span id="L36" rel="#L36">36</span>
<span id="L37" rel="#L37">37</span>
<span id="L38" rel="#L38">38</span>
<span id="L39" rel="#L39">39</span>
<span id="L40" rel="#L40">40</span>
<span id="L41" rel="#L41">41</span>
<span id="L42" rel="#L42">42</span>
<span id="L43" rel="#L43">43</span>
<span id="L44" rel="#L44">44</span>
<span id="L45" rel="#L45">45</span>
<span id="L46" rel="#L46">46</span>
<span id="L47" rel="#L47">47</span>
<span id="L48" rel="#L48">48</span>
<span id="L49" rel="#L49">49</span>
<span id="L50" rel="#L50">50</span>
<span id="L51" rel="#L51">51</span>
<span id="L52" rel="#L52">52</span>
<span id="L53" rel="#L53">53</span>
<span id="L54" rel="#L54">54</span>
<span id="L55" rel="#L55">55</span>
<span id="L56" rel="#L56">56</span>
<span id="L57" rel="#L57">57</span>
<span id="L58" rel="#L58">58</span>
<span id="L59" rel="#L59">59</span>
<span id="L60" rel="#L60">60</span>
<span id="L61" rel="#L61">61</span>
<span id="L62" rel="#L62">62</span>
<span id="L63" rel="#L63">63</span>
<span id="L64" rel="#L64">64</span>
<span id="L65" rel="#L65">65</span>
<span id="L66" rel="#L66">66</span>
<span id="L67" rel="#L67">67</span>
<span id="L68" rel="#L68">68</span>
<span id="L69" rel="#L69">69</span>
<span id="L70" rel="#L70">70</span>
<span id="L71" rel="#L71">71</span>
<span id="L72" rel="#L72">72</span>
<span id="L73" rel="#L73">73</span>
<span id="L74" rel="#L74">74</span>
<span id="L75" rel="#L75">75</span>
<span id="L76" rel="#L76">76</span>
<span id="L77" rel="#L77">77</span>
<span id="L78" rel="#L78">78</span>
<span id="L79" rel="#L79">79</span>
<span id="L80" rel="#L80">80</span>
<span id="L81" rel="#L81">81</span>
<span id="L82" rel="#L82">82</span>
<span id="L83" rel="#L83">83</span>
<span id="L84" rel="#L84">84</span>
<span id="L85" rel="#L85">85</span>
<span id="L86" rel="#L86">86</span>
<span id="L87" rel="#L87">87</span>
<span id="L88" rel="#L88">88</span>
<span id="L89" rel="#L89">89</span>
<span id="L90" rel="#L90">90</span>
<span id="L91" rel="#L91">91</span>
<span id="L92" rel="#L92">92</span>
<span id="L93" rel="#L93">93</span>
<span id="L94" rel="#L94">94</span>

          </td>
          <td class="blob-line-code">
                  <div class="highlight"><pre><div class='line' id='LC1'><span class="cp">#ifndef restricted_boltzmann_machine_H </span></div><div class='line' id='LC2'><span class="cp">#define restricted_boltzmann_machine_H</span></div><div class='line' id='LC3'><span class="cp">#include &quot;matrix_functions.h&quot;</span></div><div class='line' id='LC4'><br/></div><div class='line' id='LC5'><span class="k">typedef</span> <span class="k">struct</span> <span class="p">{</span></div><div class='line' id='LC6'>&nbsp;&nbsp;<span class="c1">// Basic description of a restricted Boltzman machine.</span></div><div class='line' id='LC7'>&nbsp;&nbsp;<span class="kt">int</span> <span class="n">n_inputs</span><span class="p">;</span>           <span class="c1">// Number of nodes in the lower &#39;input&#39; layer.</span></div><div class='line' id='LC8'>&nbsp;&nbsp;<span class="kt">int</span> <span class="n">n_outputs</span><span class="p">;</span>          <span class="c1">// Number of nodes in the upper &#39;output&#39; layer.</span></div><div class='line' id='LC9'>&nbsp;&nbsp;</div><div class='line' id='LC10'>&nbsp;&nbsp;<span class="kt">matrix_t</span> <span class="o">*</span><span class="n">io_weights</span><span class="p">;</span>   <span class="c1">// n_inputs x n_outputs matrix representing weights between input and output nodes.</span></div><div class='line' id='LC11'>&nbsp;&nbsp;<span class="kt">double</span> <span class="o">*</span><span class="n">bias_inputs</span><span class="p">;</span>    <span class="c1">// 1x n_inputs vector representing the &#39;bias&#39; toward activation of the input nodes.</span></div><div class='line' id='LC12'>&nbsp;&nbsp;<span class="kt">double</span> <span class="o">*</span><span class="n">bias_outputs</span><span class="p">;</span>   <span class="c1">// 1x n_outputs vector representing the &#39;bias&#39; toward activation of the input nodes.</span></div><div class='line' id='LC13'><br/></div><div class='line' id='LC14'>&nbsp;&nbsp;<span class="c1">// Basic learning parameters.</span></div><div class='line' id='LC15'>&nbsp;&nbsp;<span class="kt">double</span> <span class="n">learning_rate</span><span class="p">;</span>   <span class="c1">// Rate at which the model learns.</span></div><div class='line' id='LC16'>&nbsp;&nbsp;<span class="kt">int</span> <span class="n">cd_n</span><span class="p">;</span>               <span class="c1">// Specifies the number of Gibbs sampling steps used for contrastive divergence during training.</span></div><div class='line' id='LC17'>&nbsp;&nbsp;<span class="kt">int</span> <span class="n">batch_size</span><span class="p">;</span>         <span class="c1">// Specifies the batch size for training.</span></div><div class='line' id='LC18'>&nbsp;&nbsp;<span class="kt">int</span> <span class="n">update_input_bias</span><span class="p">;</span>  <span class="c1">// Specifies whether input biases are updated during learning, or fixed (e.g. from a previous layer).</span></div><div class='line' id='LC19'>&nbsp;&nbsp;</div><div class='line' id='LC20'>&nbsp;&nbsp;<span class="c1">// Special learning options.  These are NOT guaranteed to be set.</span></div><div class='line' id='LC21'>&nbsp;&nbsp;<span class="c1">// See in: http://www.cs.utoronto.ca/~ilya/pubs/ilya_sutskever_phd_thesis.pdf; pp. 75; also see: pp.5(background),73(Adapting Nesterov methods).</span></div><div class='line' id='LC22'>&nbsp;&nbsp;<span class="kt">int</span> <span class="n">use_momentum</span><span class="p">;</span>       <span class="c1">// Use momentum during fitting.</span></div><div class='line' id='LC23'>&nbsp;&nbsp;<span class="kt">double</span> <span class="n">momentum_decay</span><span class="p">;</span>  <span class="c1">// \Mu; Rate at which old gradients are discarded.</span></div><div class='line' id='LC24'>&nbsp;&nbsp;<span class="kt">matrix_t</span> <span class="o">*</span><span class="n">momentum</span><span class="p">;</span>     <span class="c1">// Momentum term; serves as memory for other mini-batch members.  Speeds the rate of convergence.</span></div><div class='line' id='LC25'>&nbsp;&nbsp;<span class="kt">double</span> <span class="o">*</span><span class="n">input_momentum</span><span class="p">;</span> <span class="c1">// Momentum term for input biases.</span></div><div class='line' id='LC26'>&nbsp;&nbsp;<span class="kt">double</span> <span class="o">*</span><span class="n">output_momentum</span><span class="p">;</span><span class="c1">// Momentum term for output biases.</span></div><div class='line' id='LC27'><br/></div><div class='line' id='LC28'>&nbsp;&nbsp;<span class="kt">int</span> <span class="n">use_l2_penalty</span><span class="p">;</span>     <span class="c1">// Usees L2 regularization to constrain weight updates.</span></div><div class='line' id='LC29'>&nbsp;&nbsp;<span class="kt">double</span> <span class="n">weight_cost</span><span class="p">;</span>     <span class="c1">// Alpha term in the L2 regularization function.</span></div><div class='line' id='LC30'><br/></div><div class='line' id='LC31'>&nbsp;&nbsp;<span class="c1">// Also implement sparsity controls(?!).</span></div><div class='line' id='LC32'><span class="p">}</span> <span class="kt">rbm_t</span><span class="p">;</span></div><div class='line' id='LC33'><br/></div><div class='line' id='LC34'><span class="k">typedef</span> <span class="k">struct</span> <span class="p">{</span></div><div class='line' id='LC35'>&nbsp;&nbsp;<span class="c1">// Abstraction used to store the graident at a point in both RMBs and DBNs.</span></div><div class='line' id='LC36'>&nbsp;&nbsp;<span class="kt">matrix_t</span> <span class="o">*</span><span class="n">delta_w</span><span class="p">;</span></div><div class='line' id='LC37'>&nbsp;&nbsp;<span class="kt">double</span> <span class="o">*</span><span class="n">delta_output_bias</span><span class="p">;</span></div><div class='line' id='LC38'>&nbsp;&nbsp;</div><div class='line' id='LC39'>&nbsp;&nbsp;<span class="kt">int</span> <span class="n">input_bias_allocated</span><span class="p">;</span> <span class="c1">// Logical; determines whether or not to update input biases.  Prevents input_biases from being updated by a dbn during backpropagation.</span></div><div class='line' id='LC40'>&nbsp;&nbsp;<span class="kt">double</span> <span class="o">*</span><span class="n">delta_input_bias</span><span class="p">;</span></div><div class='line' id='LC41'>&nbsp;&nbsp;</div><div class='line' id='LC42'>&nbsp;&nbsp;<span class="c1">// Applying delta_w uses the parameters specified here.</span></div><div class='line' id='LC43'>&nbsp;&nbsp;<span class="kt">int</span> <span class="n">batch_size</span><span class="p">;</span></div><div class='line' id='LC44'><span class="p">}</span> <span class="kt">delta_w_t</span><span class="p">;</span></div><div class='line' id='LC45'><br/></div><div class='line' id='LC46'><span class="cp">#define rbm_sample_state(prob) (prob&gt;runif(0.0, 1.0)?1:0)</span></div><div class='line' id='LC47'><span class="cp">#define logistic_function(value) (1/ (1+exp(-value)))</span></div><div class='line' id='LC48'><br/></div><div class='line' id='LC49'><span class="kr">inline</span> <span class="kt">void</span> <span class="nf">clamp_input</span><span class="p">(</span><span class="kt">rbm_t</span> <span class="o">*</span><span class="n">rbm</span><span class="p">,</span> <span class="kt">double</span> <span class="o">*</span><span class="n">input</span><span class="p">,</span> <span class="kt">double</span> <span class="o">*</span><span class="n">resulting_output</span><span class="p">)</span> <span class="p">{</span></div><div class='line' id='LC50'>&nbsp;&nbsp;<span class="k">for</span><span class="p">(</span><span class="kt">int</span> <span class="n">i</span><span class="o">=</span><span class="mi">0</span><span class="p">;</span><span class="n">i</span><span class="o">&lt;</span><span class="n">rbm</span><span class="p">[</span><span class="mi">0</span><span class="p">].</span><span class="n">n_outputs</span><span class="p">;</span><span class="n">i</span><span class="o">++</span><span class="p">)</span> <span class="p">{</span><span class="c1">// Get prob. of input node by summing over output states.</span></div><div class='line' id='LC51'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">resulting_output</span><span class="p">[</span><span class="n">i</span><span class="p">]</span><span class="o">=</span> <span class="n">rbm</span><span class="p">[</span><span class="mi">0</span><span class="p">].</span><span class="n">bias_outputs</span><span class="p">[</span><span class="n">i</span><span class="p">];</span></div><div class='line' id='LC52'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">for</span><span class="p">(</span><span class="kt">int</span> <span class="n">j</span><span class="o">=</span><span class="mi">0</span><span class="p">;</span><span class="n">j</span><span class="o">&lt;</span><span class="n">rbm</span><span class="p">[</span><span class="mi">0</span><span class="p">].</span><span class="n">n_inputs</span><span class="p">;</span><span class="n">j</span><span class="o">++</span><span class="p">)</span> <span class="p">{</span></div><div class='line' id='LC53'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">resulting_output</span><span class="p">[</span><span class="n">i</span><span class="p">]</span><span class="o">+=</span> <span class="n">input</span><span class="p">[</span><span class="n">j</span><span class="p">]</span><span class="o">*</span><span class="n">get_matrix_value</span><span class="p">(</span><span class="n">rbm</span><span class="p">[</span><span class="mi">0</span><span class="p">].</span><span class="n">io_weights</span><span class="p">,</span> <span class="n">i</span><span class="p">,</span> <span class="n">j</span><span class="p">);</span></div><div class='line' id='LC54'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="p">}</span></div><div class='line' id='LC55'>	<span class="n">resulting_output</span><span class="p">[</span><span class="n">i</span><span class="p">]</span><span class="o">=</span> <span class="n">logistic_function</span><span class="p">(</span><span class="n">resulting_output</span><span class="p">[</span><span class="n">i</span><span class="p">]);</span></div><div class='line' id='LC56'>&nbsp;&nbsp;<span class="p">}</span></div><div class='line' id='LC57'><span class="p">}</span></div><div class='line' id='LC58'><br/></div><div class='line' id='LC59'><span class="kr">inline</span> <span class="kt">void</span> <span class="nf">clamp_output</span><span class="p">(</span><span class="kt">rbm_t</span> <span class="o">*</span><span class="n">rbm</span><span class="p">,</span> <span class="kt">double</span> <span class="o">*</span><span class="n">output</span><span class="p">,</span> <span class="kt">double</span> <span class="o">*</span><span class="n">resulting_input</span><span class="p">)</span>  <span class="p">{</span></div><div class='line' id='LC60'>&nbsp;&nbsp;<span class="k">for</span><span class="p">(</span><span class="kt">int</span> <span class="n">i</span><span class="o">=</span><span class="mi">0</span><span class="p">;</span><span class="n">i</span><span class="o">&lt;</span><span class="n">rbm</span><span class="p">[</span><span class="mi">0</span><span class="p">].</span><span class="n">n_inputs</span><span class="p">;</span><span class="n">i</span><span class="o">++</span><span class="p">)</span> <span class="p">{</span><span class="c1">// Get prob. of input node by summing over output states.</span></div><div class='line' id='LC61'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">resulting_input</span><span class="p">[</span><span class="n">i</span><span class="p">]</span><span class="o">=</span> <span class="n">rbm</span><span class="p">[</span><span class="mi">0</span><span class="p">].</span><span class="n">bias_inputs</span><span class="p">[</span><span class="n">i</span><span class="p">];</span></div><div class='line' id='LC62'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">for</span><span class="p">(</span><span class="kt">int</span> <span class="n">j</span><span class="o">=</span><span class="mi">0</span><span class="p">;</span><span class="n">j</span><span class="o">&lt;</span><span class="n">rbm</span><span class="p">[</span><span class="mi">0</span><span class="p">].</span><span class="n">n_outputs</span><span class="p">;</span><span class="n">j</span><span class="o">++</span><span class="p">)</span> <span class="p">{</span></div><div class='line' id='LC63'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">resulting_input</span><span class="p">[</span><span class="n">i</span><span class="p">]</span><span class="o">+=</span> <span class="n">output</span><span class="p">[</span><span class="n">j</span><span class="p">]</span><span class="o">*</span><span class="n">get_matrix_value</span><span class="p">(</span><span class="n">rbm</span><span class="p">[</span><span class="mi">0</span><span class="p">].</span><span class="n">io_weights</span><span class="p">,</span> <span class="n">j</span><span class="p">,</span> <span class="n">i</span><span class="p">);</span></div><div class='line' id='LC64'>	<span class="p">}</span></div><div class='line' id='LC65'>	<span class="n">resulting_input</span><span class="p">[</span><span class="n">i</span><span class="p">]</span><span class="o">=</span> <span class="n">logistic_function</span><span class="p">(</span><span class="n">resulting_input</span><span class="p">[</span><span class="n">i</span><span class="p">]);</span></div><div class='line' id='LC66'>&nbsp;&nbsp;<span class="p">}</span></div><div class='line' id='LC67'><span class="p">}</span></div><div class='line' id='LC68'><br/></div><div class='line' id='LC69'><span class="kt">rbm_t</span> <span class="o">*</span><span class="nf">alloc_rbm</span><span class="p">(</span><span class="kt">int</span> <span class="n">n_inputs</span><span class="p">,</span> <span class="kt">int</span> <span class="n">n_outputs</span><span class="p">);</span></div><div class='line' id='LC70'><span class="kt">void</span> <span class="nf">free_rbm</span><span class="p">(</span><span class="kt">rbm_t</span> <span class="o">*</span><span class="n">rbm</span><span class="p">);</span></div><div class='line' id='LC71'><span class="kt">void</span> <span class="nf">init_rbm</span><span class="p">(</span><span class="kt">rbm_t</span> <span class="o">*</span><span class="n">rbm</span><span class="p">,</span> <span class="kt">double</span> <span class="n">learning_rate</span><span class="p">,</span> <span class="kt">int</span> <span class="n">batch_size</span><span class="p">,</span> <span class="kt">int</span> <span class="n">cd_n</span><span class="p">,</span> <span class="kt">double</span> <span class="n">expected_frequency_on</span><span class="p">);</span></div><div class='line' id='LC72'><br/></div><div class='line' id='LC73'><span class="kt">void</span> <span class="nf">apply_delta_w</span><span class="p">(</span><span class="kt">rbm_t</span> <span class="o">*</span><span class="n">rbm</span><span class="p">,</span> <span class="kt">delta_w_t</span> <span class="o">*</span><span class="n">dw</span><span class="p">);</span></div><div class='line' id='LC74'><span class="kt">void</span> <span class="nf">initial_momentum_step</span><span class="p">(</span><span class="kt">rbm_t</span> <span class="o">*</span><span class="n">rbm</span><span class="p">);</span></div><div class='line' id='LC75'><span class="kt">void</span> <span class="nf">apply_momentum_correction</span><span class="p">(</span><span class="kt">rbm_t</span> <span class="o">*</span><span class="n">rbm</span><span class="p">,</span> <span class="kt">delta_w_t</span> <span class="o">*</span><span class="n">dw</span><span class="p">);</span></div><div class='line' id='LC76'><br/></div><div class='line' id='LC77'><span class="kt">void</span> <span class="nf">free_delta_w_ptr</span><span class="p">(</span><span class="kt">delta_w_t</span> <span class="o">*</span><span class="n">dw</span><span class="p">,</span> <span class="kt">int</span> <span class="n">n</span><span class="p">);</span></div><div class='line' id='LC78'><span class="kt">void</span> <span class="nf">rbm_train</span><span class="p">(</span><span class="kt">rbm_t</span> <span class="o">*</span><span class="n">rbm</span><span class="p">,</span> <span class="kt">double</span> <span class="o">*</span><span class="n">input_example</span><span class="p">,</span> <span class="kt">int</span> <span class="n">n_examples</span><span class="p">,</span> <span class="kt">int</span> <span class="n">n_epocs</span><span class="p">,</span> <span class="kt">int</span> <span class="n">n_threads</span><span class="p">);</span></div><div class='line' id='LC79'><br/></div><div class='line' id='LC80'><span class="kt">delta_w_t</span> <span class="o">*</span><span class="nf">alloc_dwt_from_rbm</span><span class="p">(</span><span class="kt">rbm_t</span> <span class="o">*</span><span class="n">rbm</span><span class="p">);</span></div><div class='line' id='LC81'><br/></div><div class='line' id='LC82'><span class="kt">rbm_t</span> <span class="o">*</span><span class="nf">rbm_r_to_c</span><span class="p">(</span><span class="n">SEXP</span> <span class="n">rbm_r</span><span class="p">);</span></div><div class='line' id='LC83'><span class="kt">rbm_t</span> <span class="o">*</span><span class="nf">rbm_layer_r_to_c</span><span class="p">(</span><span class="n">SEXP</span> <span class="n">rbm_r</span><span class="p">,</span> <span class="kt">double</span> <span class="o">*</span><span class="n">points_to_bias_inputs</span><span class="p">);</span></div><div class='line' id='LC84'><br/></div><div class='line' id='LC85'><span class="n">SEXP</span> <span class="nf">train_rbm_R</span><span class="p">(</span><span class="n">SEXP</span> <span class="n">rbm_r</span><span class="p">,</span> <span class="n">SEXP</span> <span class="n">training_data_r</span><span class="p">,</span> <span class="n">SEXP</span> <span class="n">n_epocs_r</span><span class="p">,</span> <span class="n">SEXP</span> <span class="n">n_threads_r</span><span class="p">);</span></div><div class='line' id='LC86'><br/></div><div class='line' id='LC87'><span class="c1">// MUTEX for training...</span></div><div class='line' id='LC88'><span class="cp">#ifndef Win32</span></div><div class='line' id='LC89'><span class="cp">#include &lt;pthread.h&gt;</span></div><div class='line' id='LC90'><span class="kt">pthread_mutex_t</span> <span class="n">rbm_mutex</span><span class="p">;</span></div><div class='line' id='LC91'><span class="cp">#endif</span></div><div class='line' id='LC92'><br/></div><div class='line' id='LC93'><span class="cp">#endif</span></div><div class='line' id='LC94'><br/></div></pre></div>
          </td>
        </tr>
      </table>
  </div>

          </div>
        </div>

        <a href="#jump-to-line" rel="facebox" data-hotkey="l" class="js-jump-to-line" style="display:none">Jump to Line</a>
        <div id="jump-to-line" style="display:none">
          <h2>Jump to Line</h2>
          <form accept-charset="UTF-8" class="js-jump-to-line-form">
            <input class="textfield js-jump-to-line-field" type="text">
            <div class="full-button">
              <button type="submit" class="button">Go</button>
            </div>
          </form>
        </div>

      </div>
    </div>
</div>

<div id="js-frame-loading-template" class="frame frame-loading large-loading-area" style="display:none;">
  <img class="js-frame-loading-spinner" src="https://a248.e.akamai.net/assets.github.com/images/spinners/octocat-spinner-128.gif?1347543528" height="64" width="64">
</div>


        </div>
      </div>
      <div class="context-overlay"></div>
    </div>

      <div id="footer-push"></div><!-- hack for sticky footer -->
    </div><!-- end of wrapper - hack for sticky footer -->

      <!-- footer -->
      <div id="footer">
  <div class="container clearfix">

      <dl class="footer_nav">
        <dt>GitHub</dt>
        <dd><a href="https://github.com/about">About us</a></dd>
        <dd><a href="https://github.com/blog">Blog</a></dd>
        <dd><a href="https://github.com/contact">Contact &amp; support</a></dd>
        <dd><a href="http://enterprise.github.com/">GitHub Enterprise</a></dd>
        <dd><a href="http://status.github.com/">Site status</a></dd>
      </dl>

      <dl class="footer_nav">
        <dt>Applications</dt>
        <dd><a href="http://mac.github.com/">GitHub for Mac</a></dd>
        <dd><a href="http://windows.github.com/">GitHub for Windows</a></dd>
        <dd><a href="http://eclipse.github.com/">GitHub for Eclipse</a></dd>
        <dd><a href="http://mobile.github.com/">GitHub mobile apps</a></dd>
      </dl>

      <dl class="footer_nav">
        <dt>Services</dt>
        <dd><a href="http://get.gaug.es/">Gauges: Web analytics</a></dd>
        <dd><a href="http://speakerdeck.com">Speaker Deck: Presentations</a></dd>
        <dd><a href="https://gist.github.com">Gist: Code snippets</a></dd>
        <dd><a href="http://jobs.github.com/">Job board</a></dd>
      </dl>

      <dl class="footer_nav">
        <dt>Documentation</dt>
        <dd><a href="http://help.github.com/">GitHub Help</a></dd>
        <dd><a href="http://developer.github.com/">Developer API</a></dd>
        <dd><a href="http://github.github.com/github-flavored-markdown/">GitHub Flavored Markdown</a></dd>
        <dd><a href="http://pages.github.com/">GitHub Pages</a></dd>
      </dl>

      <dl class="footer_nav">
        <dt>More</dt>
        <dd><a href="http://training.github.com/">Training</a></dd>
        <dd><a href="https://github.com/edu">Students &amp; teachers</a></dd>
        <dd><a href="http://shop.github.com">The Shop</a></dd>
        <dd><a href="/plans">Plans &amp; pricing</a></dd>
        <dd><a href="http://octodex.github.com/">The Octodex</a></dd>
      </dl>

      <hr class="footer-divider">


    <p class="right">&copy; 2013 <span title="0.12473s from fe1.rs.github.com">GitHub</span>, Inc. All rights reserved.</p>
    <a class="left" href="https://github.com/">
      <span class="mega-icon mega-icon-invertocat"></span>
    </a>
    <ul id="legal">
        <li><a href="https://github.com/site/terms">Terms of Service</a></li>
        <li><a href="https://github.com/site/privacy">Privacy</a></li>
        <li><a href="https://github.com/security">Security</a></li>
    </ul>

  </div><!-- /.container -->

</div><!-- /.#footer -->


    <div class="fullscreen-overlay js-fullscreen-overlay" id="fullscreen_overlay">
  <div class="fullscreen-container js-fullscreen-container">
    <div class="textarea-wrap">
      <textarea name="fullscreen-contents" id="fullscreen-contents" class="js-fullscreen-contents" placeholder="" data-suggester="fullscreen_suggester"></textarea>
          <div class="suggester-container">
              <div class="suggester fullscreen-suggester js-navigation-container" id="fullscreen_suggester"
                 data-url="/dankoc/Rdbn/suggestions/commit">
              </div>
          </div>
    </div>
  </div>
  <div class="fullscreen-sidebar">
    <a href="#" class="exit-fullscreen js-exit-fullscreen tooltipped leftwards" title="Exit Zen Mode">
      <span class="mega-icon mega-icon-normalscreen"></span>
    </a>
    <a href="#" class="theme-switcher js-theme-switcher tooltipped leftwards"
      title="Switch themes">
      <span class="mini-icon mini-icon-brightness"></span>
    </a>
  </div>
</div>



    <div id="ajax-error-message" class="flash flash-error">
      <span class="mini-icon mini-icon-exclamation"></span>
      Something went wrong with that request. Please try again.
      <a href="#" class="mini-icon mini-icon-remove-close ajax-error-dismiss"></a>
    </div>

    
    
    <span id='server_response_time' data-time='0.12521' data-host='fe1'></span>
    
  </body>
</html>

